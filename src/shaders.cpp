
#include "shaders.hpp"
#include <cstdio>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iterator>
#include <algorithm>

std::vector<Shader*> shaders;
std::vector<Program*> programs;


Shader::Shader() {
	shaders.push_back(this);
}

void Shader::load_from_file(std::string filename, Shader::Type type) {
	FILE* file;
	if (name.empty())
		name = filename;
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


void GuiShaderTypeComboBox(std::string label, Shader::Type& prop) {
	static std::vector<const char*> labels;
	static std::vector<int> values;

	static bool initialized = false;

	if (!initialized) {
		initialized = true;
		for (const auto& pair : Shader::TypeNames) {
			labels.push_back(pair.second.c_str());
			values.push_back(static_cast<int>(pair.first));
		}
	}

	int current = std::find(values.begin(), values.end(), static_cast<int>(prop)) - values.begin();
	if(ImGui::Combo(label.c_str(), &current, &labels[0], (int)labels.size()))
		prop = static_cast<Shader::Type>(values[current]);
	
}


//TODO: Move to another file, and clean up
// Adds a string property to a property list
void GuiProperty(std::string label, std::string& prop) {

	constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

	ImGui::PushID(label.c_str());
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);

	ImGui::InputText("", &prop);
	ImGui::NextColumn();
	ImGui::PopID();

	ImGui::TableNextRow();
}

// Adds a shader type property to a property list
void GuiProperty(std::string label, Shader::Type& prop) {

	constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

	ImGui::PushID(label.c_str());
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);

	GuiShaderTypeComboBox("", prop);
	ImGui::NextColumn();
	ImGui::PopID();

	ImGui::TableNextRow();
}

// Adds a float property to a property list
void GuiProperty(std::string label, float& prop) {

	constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

	ImGui::PushID(label.c_str());
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);

	ImGui::DragFloat("", &prop, .1);
	ImGui::NextColumn();
	ImGui::PopID();

	ImGui::TableNextRow();
}

// Adds a read only property to a property list
void GuiPropertyRO(std::string label, std::string prop) {

	constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

	ImGui::PushID(label.c_str());
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);

	ImGui::Text(prop.c_str());
	ImGui::NextColumn();
	ImGui::PopID();

	ImGui::TableNextRow();
}

void Shader::render_gui_segment() {
	ImGui::PushID(this);

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);


	bool open = ImGui::TreeNode("Shader", name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		Shader* shader = this;
		ImGui::SetDragDropPayload("SHADER", &shader, sizeof(Shader*));
		ImGui::Text(filename.c_str());
		ImGui::EndDragDropSource();
	}


	ImGui::AlignTextToFramePadding();

	if (open) {
		ImGui::TableNextRow();

		GuiProperty("Name", name);
		GuiProperty("Filename", filename);
		GuiProperty("Type", type);
		GuiPropertyRO("Compiled", compiled ? "True" : "False");


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);


		if (ImGui::Button("Info Log")) {
			show_info_log = true;
		}

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

	//Now, show the info logs, if open
	for (auto& shader : shaders) {
		if (shader->show_info_log) {
			shader->render_info_log();
		}
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
	fragment_shader = nullptr;
}


void Program::link() {
	program_id = glCreateProgram();

	if (fragment_shader) glAttachShader(program_id, fragment_shader->shader_id);
	if (vertex_shader) glAttachShader(program_id, vertex_shader->shader_id);


	glLinkProgram(program_id);

	GLint _linked;
	glGetProgramiv(program_id, GL_LINK_STATUS, &_linked);

	if (_linked) linked = true;

	int32_t log_length = 0;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
	info_log.resize(log_length);
	glGetShaderInfoLog(program_id, log_length, &log_length, &info_log[0]);

	if (linked) {
		// Query the program for its uniforms
		int num_active_uniforms = 0;
		glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms);

		float_uniform_values.resize(num_active_uniforms);

		for (int i = 0; i < num_active_uniforms; i++) {
			char uniform_name[1024];
			int length;
			uint32_t uniform_type;
			int uniform_size;

			glGetActiveUniform(program_id, i, 1024, &length, &uniform_size, &uniform_type, uniform_name);

			switch (uniform_type) {
			case GL_FLOAT:
				float_uniforms.push_back(uniform_name);

			}
		}
		printf("%d\n", num_active_uniforms);
	}
}

void Program::render_gui_segment() {
	ImGui::PushID(this);

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);


	bool open = ImGui::TreeNode("Program", name.c_str());

	ImGui::AlignTextToFramePadding();

	if (open) {
		ImGui::TableNextRow();

		GuiProperty("Name", name);
		GuiPropertyRO("Linked", linked ? "True" : "False");


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

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


		ImGui::Text(vertex_shader ? vertex_shader->name.c_str() : "");


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("Fragment Shader");

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHADER")) {
				fragment_shader = *(Shader**)payload->Data;
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::TableNextColumn();


		ImGui::Text(fragment_shader ? fragment_shader->name.c_str() : "");

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNode("Uniform", "Uniforms")) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			for (int i = 0; i < float_uniforms.size(); i++) {
				GuiProperty(float_uniforms[i], float_uniform_values[i]);
			}

			ImGui::TreePop();
		}



		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		if (ImGui::Button("Link"))
			link();


		if (ImGui::Button("Info Log")) {
			show_info_log = true;
		}



		ImGui::TreePop();
	}

	ImGui::PopID();
}
/*
void Program::render_gui_segment() {
	ImGui::PushID(this);

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

			
			ImGui::Text(vertex_shader ? vertex_shader->name.c_str() : "");
			

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::Text("Fragment Shader");

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHADER")) {
					fragment_shader = *(Shader**)payload->Data;
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::TableNextColumn();


			ImGui::Text(fragment_shader ? fragment_shader->name.c_str() : "");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			bool open = ImGui::TreeNode("Uniform", "Uniforms");

			

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(1);

			if (ImGui::Button("Link"))
				link();

			ImGui::EndTable();
		}
	}

	ImGui::PopID();
}*/

void Program::show_programs_gui(bool& show) {
	if (show && ImGui::Begin("Programs", &show)) {
		if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {

			for (auto& program : programs) {
				program->render_gui_segment();
			}

			ImGui::EndTable();
		}

		
		ImGui::End();
	}
}