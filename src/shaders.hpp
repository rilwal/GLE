#pragma once

#include <glad/gl.h>
#include <string>
#include <vector>
#include <map>

struct Shader;



struct Shader {
	enum class Type {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	const std::map<Type, std::string> TypeNames = {
		{Type::VERTEX, "Vertex"},
		{Type::FRAGMENT, "Fragment"}
	};

	// static section deals with all shaders
	static void show_shaders_gui(bool& show);


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
	std::string name;

	Shader* vertex_shader;
	Shader* fragment_shader;

	uint32_t program_id;
	std::string info_log;

	static void show_programs_gui(bool& show);

	Program();

	void render_gui_segment();
	void link(std::vector<Shader>& shaders);
};

