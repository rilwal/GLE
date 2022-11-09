
#include "shaders.hpp"

#include <iterator>
#include <algorithm>

#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "property_list.hpp"


std::vector<Shader*> shaders;
std::vector<Program*> programs;


Shader::Shader() {
	shader_id = 0;
	type = Shader::Type::FRAGMENT;
	shaders.push_back(this);
}


void Shader::load_from_file(std::string filename, Shader::Type type) {
	FILE* file;
	if (name.empty())
		name = filename;
	this->filename = filename;
	this->type = type;

#ifdef _WIN32
	errno_t error = fopen_s(&file, filename.c_str(), "rb");
	
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


	std::vector<char> shader_src;
	shader_src.resize(filesize + 2);


	filesize = fread(shader_src.data(), 1, filesize, file);
	fclose(file);

	shader_src[filesize] = 0;
	shader_src[filesize + 1] = 0;


	printf("Shader source: %s\n", shader_src.data());

	shader_id = glCreateShader((GLenum)type);
	const char* source_cstr = shader_src.data();
	int32_t source_len = shader_src.size();
	glShaderSource(shader_id, 1, &source_cstr, &source_len);

	compiled = false;

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


	bool open = ImGui::BeginPropListEntry(name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		Shader* shader = this;
		ImGui::SetDragDropPayload("SHADER", &shader, sizeof(Shader*));
		ImGui::Text(filename.c_str());
		ImGui::EndDragDropSource();
	}

	ImGui::TableNextRow();


	if (open) {

		ImGui::PLProp("Name", name);
		ImGui::PLProp("Filename", filename);
		ImGui::PLProp("Type", type);
		ImGui::PLInfo("Compiled", compiled);


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		if (ImGui::Button("Reload")) {
			load_from_file(filename, type);
		}

		if (ImGui::Button("Info Log")) {
			show_info_log = true;
		}

		ImGui::EndPropListEntry();
	}

	ImGui::PopID();
}

void Shader::show_shaders_gui(bool& show) {
	static bool show_create_shader_window = false;

	// Main shaders window
	if (show && ImGui::Begin("Shaders", &show)) {
		
		// First, show the table of existing shaders
		if (ImGui::BeginPropList("Shaders")) {

			for (auto& shader : shaders) {
				shader->render_gui_segment();
			}

			ImGui::EndPropList();
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
	if (compute_shader) glAttachShader(program_id, compute_shader->shader_id);

	linked = false;

	glLinkProgram(program_id);

	if (fragment_shader) glDetachShader(program_id, fragment_shader->shader_id);
	if (vertex_shader) glDetachShader(program_id, vertex_shader->shader_id);

	GLint _linked;
	glGetProgramiv(program_id, GL_LINK_STATUS, &_linked);

	if (_linked) linked = true;

	if (!linked) {
		int32_t log_length = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
		info_log.resize(log_length);
		glGetProgramInfoLog(program_id, log_length, &log_length, info_log.data());// &info_log[0]);

		printf("%s\n", info_log);
	}

	if (linked) {
		// Query the program for its uniforms
		int num_active_uniforms = 0;
		glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms);
		
		std::map<std::string, Uniform> _uniforms;


		for (int i = 0; i < num_active_uniforms; i++) {
			char uniform_name[1024];
			int length;
			uint32_t uniform_type;
			int uniform_size;

			glGetActiveUniform(program_id, i, 1024, &length, &uniform_size, &uniform_type, uniform_name);
			Uniform u;
			u.location = i;
			u.name = uniform_name;
			u.show_in_ui = !u.name.starts_with("_");

			switch (uniform_type) {
			case GL_FLOAT:
				u.type = Uniform::Type::Float;

				
				glGetUniformfv(program_id, i, &u.value.f);
				break;
			case GL_FLOAT_VEC3:
				// if the name starts with c_, treat it as a color
				if (u.name.starts_with("c_")) {
					u.type = Uniform::Type::Color3;
					u.name = u.name.substr(2);
				}
				else {
					u.type = Uniform::Type::Vec3;
				}

				glGetUniformfv(program_id, i, &u.value.f);
				break;
			case GL_FLOAT_MAT4:
				u.value.m4 = glm::identity<glm::mat4>();
				u.type = Uniform::Type::Mat4;
				break;

			case GL_SAMPLER_2D:
				u.value.i = 0;
				u.type = Uniform::Type::Texture2D;
				break;
			
			case GL_SAMPLER_CUBE:
				u.value.i = 0;
				u.type = Uniform::Type::TextureCube;
				break;

			case GL_IMAGE_2D:
				u.value.i = 0;
				u.type = Uniform::Type::Image2D;
				break;

			}

			// persist uniform values across reloads
			if (uniforms.contains(u.name) && uniforms[u.name].type == u.type) {
				u.value = uniforms[u.name].value;
			}

			_uniforms[u.name] = u;
		}

		uniforms = _uniforms;
		for (auto& material : materials) {
			//material.uniforms = uniforms;
			std::map<std::string, Uniform> _uniforms = uniforms;

			for (auto& [name, u] : _uniforms) {
				if (material.uniforms.contains(u.name) && material.uniforms[u.name].type == u.type) {
					_uniforms[u.name].value = material.uniforms[u.name].value;
				}
			}

			material.uniforms = _uniforms;
		}
	}
}


void Program::render_gui_segment() {
	ImGui::PushID(this);

	if (ImGui::BeginPropListEntry(name.c_str())) {
		ImGui::TableNextRow();

		ImGui::PLProp("Name", name);
		ImGui::PLInfo("Linked", linked);
		ImGui::PLProp("Vertex Shader", vertex_shader);
		ImGui::PLProp("Fragment Shader", fragment_shader);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNode("Uniform", "Uniforms")) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			for (auto& [name, uniform] : uniforms) {
				if (uniform.show_in_ui) {
					switch (uniform.type) {
					case Uniform::Type::Float:
						ImGui::PLProp(uniform.name, uniform.value.f);
						break;

					case Uniform::Type::Vec3:
						ImGui::PLProp(uniform.name, uniform.value.v3);
						break;

					case Uniform::Type::Color3:
						ImGui::PLColor(uniform.name, uniform.value.v3);
						break;

					case Uniform::Type::Mat4:
						ImGui::PLProp(uniform.name, uniform.value.m4);
						break;

					case Uniform::Type::Texture2D:
					case Uniform::Type::TextureCube:
						ImGui::PLTexture(uniform.name, uniform.value.i);
						break;
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		if (ImGui::Button("Link"))
			link();


		if (ImGui::Button("Info Log"))
			show_info_log = true;

		ImGui::EndPropListEntry();
	}

	ImGui::PopID();
}


void Program::render_info_log() {
	if (show_info_log) {
		char title[1024];
		snprintf(title, 1023, "Info log for %s", name.c_str());
		if (ImGui::Begin(title, &show_info_log)) {
			ImGui::Text("Info Log:\n%s", info_log.c_str());
		}

		ImGui::End();
	}
}



void Program::show_programs_gui(bool& show) {
	if (show && ImGui::Begin("Programs", &show)) {
		if (ImGui::BeginPropList("Programs")) {

			for (auto& program : programs) {
				program->render_gui_segment();
			}

			ImGui::EndTable();
		}

		
		ImGui::End();
	}

	for (auto& program : programs) {
		if (program->show_info_log) {
			program->render_info_log();
		}
	}
}

void Program::use() {

	glUseProgram(program_id);

	for (auto& [name, uniform] : uniforms) {
		switch (uniform.type) {
		case Uniform::Type::Float:
			glUniform1f(uniform.location, uniform.value.f);
			break;

		case Uniform::Type::Color3:
		case Uniform::Type::Vec3:
			glUniform3f(uniform.location, uniform.value.v3.x, uniform.value.v3.y, uniform.value.v3.z);
			break;

		case Uniform::Type::Mat4:
			glUniformMatrix4fv(uniform.location, 1, false, &uniform.value.m4[0][0]);
			break;
		}
	}
}

size_t Program::create_material() {
	Material m = {};
	m.uniforms = uniforms;
	m.program = this;

	materials.push_back(m);
	return materials.size() - 1;
}


void Material::render_gui_segment() {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	for (auto& [name, uniform] : uniforms) {
		if (uniform.show_in_ui) {
			switch (uniform.type) {
			case Program::Uniform::Type::Float:
				ImGui::PLProp(uniform.name, uniform.value.f);
				break;

			case Program::Uniform::Type::Vec3:
				ImGui::PLProp(uniform.name, uniform.value.v3);
				break;

			case Program::Uniform::Type::Color3:
				ImGui::PLColor(uniform.name, uniform.value.v3);
				break;

			case Program::Uniform::Type::Mat4:
				ImGui::PLProp(uniform.name, uniform.value.m4);
				break;


			case Program::Uniform::Type::Texture2D:
			case Program::Uniform::Type::TextureCube:
				ImGui::PLTexture(uniform.name, uniform.value.i);
				break;
			}
		}
	}
}


void Material::use() {

	glUseProgram(program->program_id);

	uint32_t textures_activated = 0;
	uint32_t images_activated = 0;

	for (auto& [name, uniform] : uniforms) {
		switch (uniform.type) {
		case Program::Uniform::Type::Float:
			glUniform1f(uniform.location, uniform.value.f);
			break;

		case Program::Uniform::Type::Color3:
		case Program::Uniform::Type::Vec3:
			glUniform3f(uniform.location, uniform.value.v3.x, uniform.value.v3.y, uniform.value.v3.z);
			break;

		case Program::Uniform::Type::Mat4:
			glUniformMatrix4fv(uniform.location, 1, false, &uniform.value.m4[0][0]);
			break;


		case Program::Uniform::Type::TextureCube:
			glActiveTexture(GL_TEXTURE0 + textures_activated);
			glBindTexture(GL_TEXTURE_CUBE_MAP, uniform.value.i);
			glUniform1i(uniform.location, textures_activated);
			textures_activated++;
			break;


		case Program::Uniform::Type::Texture2D:
			glActiveTexture(GL_TEXTURE0 + textures_activated);
			glBindTexture(GL_TEXTURE_2D, uniform.value.i);
			glUniform1i(uniform.location, textures_activated);
			textures_activated++;
			break;
		}
	}
}

