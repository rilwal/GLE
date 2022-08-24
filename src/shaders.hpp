#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>


struct Shader {
	enum class Type {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	static inline const std::map<Type, std::string> TypeNames = {
		{Type::VERTEX, "Vertex"},
		{Type::FRAGMENT, "Fragment"}
	};

	// shows the GUI for all shaders
	static void show_shaders_gui(bool& show);


	bool compiled = false;
	Type type;
	std::string name;
	std::string filename;
	std::string source;
	std::string info_log;

	uint32_t shader_id;


	Shader();

	bool show_info_log = false;

	void load_from_file(std::string filename, Shader::Type type);

	void render_gui_segment();
	void render_info_log();
};


// Represents an OpenGL shader program, consisting of multipe linked shaders
struct Program {
	
	// Represents a uniform variable for this shader program.
	struct Uniform {
		enum class Type {
			Float,
			Integer,
			Vec2,
			Vec3,
			Color3,
			Vec4,
			Color4
		};

		// Because a uniform can have many values, we can use a union here
		// This approach is simple, but can lead to errors at times
		union {
			float f;
			int i;
			glm::vec2 v2;
			glm::vec3 v3;
			glm::vec4 v4;
		} value;

		std::string name;
		int location;
		Type type;
	};
	
	std::vector<Uniform> uniforms;

	bool linked = false;
	bool show_info_log = false;

	std::vector<Shader> shaders;
	std::string name;

	Shader* vertex_shader;
	Shader* fragment_shader;

	uint32_t program_id;
	std::string info_log;



	// TODO: Much better uniform handling
	std::vector<std::string> float_uniforms;
	std::vector<float> float_uniform_values;



	static void show_programs_gui(bool& show);

	Program();

	void render_gui_segment();
	void link();

	// Make this the active shader
	void use();
};

