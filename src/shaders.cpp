
#include "shaders.hpp"
#include <cstdio>
#include <imgui.h>
#include <iterator>
#include <algorithm>

std::vector<Shader*> shaders;
std::vector<Program*> programs;


Shader::Shader() {
	shaders.push_back(this);
}

void Shader::load_from_file(std::string filename, Shader::Type type) {
	FILE* file;
	this->filename = filename;
	this->type = type;

#ifdef _WIN32
	errno_t error = fopen_s(&file, filename.c_str(), "r");
	
	if (error != 0) {
		fprintf(stderr, "Failed to open file %s.\n", filename.c_str());
		return;
	}
#else
	file = fopen(filename.c_str(), "r");
	if (!file) {
		fprintf(stderr, "Failed to open file %s.\n", filename.c_str());
		return;
	}
#endif
	
	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);

	source.resize(filesize);
	fread(&source[0], filesize, 1, file);
	fclose(file);

	printf("Shader source: %s\n", source.c_str());

	shader_id = glCreateShader((GLenum)type);
	const char* source_cstr = source.c_str();
	int32_t source_len = source.size();
	glShaderSource(shader_id, 1, &source_cstr, &source_len);

	glCompileShader(shader_id);

	GLint _compiled;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &_compiled);

	if (_compiled) compiled = true;

	int32_t log_length = 0;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
	info_log.resize(log_length);
	glGetShaderInfoLog(shader_id, log_length, &log_length, &info_log[0]);
}

void Shader::render_gui_segment() {
	ImGui::PushID(this);

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	//bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
	bool node_open = ImGui::TreeNode(this, filename.c_str());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		Shader* shader = this;
		ImGui::SetDragDropPayload("SHADER", &shader, sizeof(Shader*));
		ImGui::Text(filename.c_str());
		ImGui::EndDragDropSource();
	}

	if (node_open) {

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("Filename:");
		ImGui::TableNextColumn();
		ImGui::Text(filename.c_str());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);


		ImGui::Text("Type:");
		ImGui::TableNextColumn();
		ImGui::Text(TypeNames.at(type).c_str());


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("Compiled:");
		ImGui::TableNextColumn();
		ImGui::Text(compiled ? "True" : "False");


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);


		if (ImGui::Button("Info Log")) show_info_log = true;

		ImGui::TreePop();
	}


	ImGui::PopID();
}

void Shader::show_shaders_gui(bool& show) {
	static bool show_create_shader_window = false;

	// Main shaders window
	if (show && ImGui::Begin("Shaders", &show)) {
		// First, show the table of existing shaders
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {

			for (auto& shader : shaders) {
				shader->render_gui_segment();
			}

			ImGui::EndTable();
		}

		// Then, render the interface to create a new shader
		if (ImGui::Button("+")) {
			show_create_shader_window = true;
		}

		ImGui::End();
	}

	if (show_create_shader_window && ImGui::Begin("Create Shader", &show_create_shader_window)) {
		static char name[1024];
		static char filename[1024];
		static Shader::Type type;
		static int selected_type;

		ImGui::InputText("Name", name, 1024);					// TODO: Default naming?
		ImGui::InputText("Filename", filename, 1024);			// TODO: File open dialog?
		
		const char* const items[2] = { "Vertex", "Fragment" };	// TODO: Generate this from the actual enum
		ImGui::Combo("Type", &selected_type, items, 2);


		if (ImGui::Button("Cancel")) {
			show_create_shader_window = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Add")) {
			Shader* s = new Shader();
			s->name = std::string(name);
			s->filename = std::string(filename);
			s->type = selected_type == 0 ? Shader::Type::VERTEX : Shader::Type::FRAGMENT; //TODO: Fix this janky hack lol

			show_create_shader_window = false;
		}

		ImGui::End();
	}
}

void Shader::render_info_log() {
	if (show_info_log) {
		char title[1024];
		snprintf(title, 1023, "Info log for %s", filename.c_str());
		if (ImGui::Begin(title, &show_info_log)) {
			ImGui::Text("Info Log:\n%s", info_log.c_str());
		}

		ImGui::End();
	}
}


Program::Program() {
	name = "Untitled Program";
	programs.push_back(this);
	vertex_shader = nullptr;
}


void Program::link(std::vector<Shader>& shaders) {
	std::copy(shaders.begin(), shaders.end(), std::back_inserter(this->shaders));
	program_id = glCreateProgram();

	for (auto& shader : shaders) {
		glAttachShader(program_id, shader.shader_id);
	}

	glLinkProgram(program_id);

	GLint _linked;
	glGetProgramiv(program_id, GL_LINK_STATUS, &_linked);

	if (_linked) linked = true;

	int32_t log_length = 0;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
	info_log.resize(log_length);
	glGetShaderInfoLog(program_id, log_length, &log_length, &info_log[0]);
}


void Program::render_gui_segment() {
	if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Linked: %s", linked ? "True" : "False");

		if (ImGui::BeginTable("Program", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::Text("Vertex Shader");

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHADER")) {
					vertex_shader = *(Shader**)payload->Data;
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::TableNextColumn();

			
			ImGui::Text(vertex_shader ? vertex_shader->filename.c_str() : "");
			




			ImGui::EndTable();
		}
	}

	for (auto& shader : shaders) {
		shader.render_info_log();
	}
}

void Program::show_programs_gui(bool& show) {
	if (ImGui::Begin("Programs", &show)) {
		for (auto& program : programs) {
			program->render_gui_segment();
		}

		
		ImGui::End();
	}
}