#pragma once

#include <glad/gl.h>
#include <string>
#include <vector>
#include <map>

struct Shader {
	enum class Type {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	const std::map<Type, std::string> TypeNames = {
		{Type::VERTEX, "Vertex"},
		{Type::FRAGMENT, "Fragment"}
	};

	bool compiled = false;
	Type type;
	std::string filename;
	std::string source;
	std::string info_log;

	uint32_t shader_id;

	bool show_info_log = false;

	void load_from_file(std::string filename, Shader::Type type);

	void render_gui_segment();
	void render_info_log();
};

struct Program {
	bool linked;
	std::vector<Shader> shaders;
	uint32_t program_id;
	std::string info_log;


	void render_gui();
	void link(std::vector<Shader>& shaders);
};
