
#include "shaders.hpp"
#include <cstdio>
#include <imgui.h>
#include <iterator>
#include <algorithm>

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
	if (ImGui::CollapsingHeader(filename.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Filename: %s", filename.c_str());
		ImGui::Text("Type: %s", TypeNames.at(type).c_str());
		ImGui::Text("Compiled: %s", compiled ? "True" : "False");
		if (ImGui::Button("Info Log")) show_info_log = true;
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


void Program::render_gui() {
	if (ImGui::CollapsingHeader("Program", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("Linked: %s", linked ? "True" : "False");

		for (auto& shader : shaders) {
			shader.render_gui_segment();
		}
	}

	for (auto& shader : shaders) {
		shader.render_info_log();
	}
}