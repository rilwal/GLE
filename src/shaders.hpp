#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

#include "asset_manager.hpp"

// Forward declarations
struct Shader;
struct Program;
struct Material;


struct Shader {
	enum class Type {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	static inline const std::map<Type, std::string> TypeNames = {
		{Type::VERTEX, "Vertex"},
		{Type::FRAGMENT, "Fragment"},
		{Type::COMPUTE, "Compute"}
	};

	// shows the GUI for all shaders
	static void show_shaders_gui(bool& show);


	bool compiled = false;
	Type type;
	std::string name;
	std::string filename;
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
			Color4,
			Mat4,
			Texture2D,
			Image2D,
			TextureCube
		};

		// Because a uniform can have many values, we can use a union here
		// This approach is simple, but can lead to errors at times so care is needed
		union {
			float f;
			int i;
			glm::vec2 v2;
			glm::vec3 v3;
			glm::vec4 v4;
			glm::mat4 m4;
		} value;

		std::string name;
		int location;
		bool show_in_ui;
		Type type;
	};
	
	std::map<std::string, Uniform> uniforms;
	std::vector<Material> materials;

	bool linked = false;
	bool show_info_log = false;

	std::vector<Shader> shaders;
	std::string name;

	Shader* vertex_shader = nullptr;
	Shader* fragment_shader = nullptr;
	Shader* compute_shader = nullptr;

	uint32_t program_id;
	std::string info_log;

	static void show_programs_gui(bool& show);

	Program();

	void render_gui_segment();
	void link();

	// Make this the active shader
	void use();

	void render_info_log();

	size_t create_material();
};


// A material is essentially a set of values for the uniforms
// in a shader program.
struct Material {
	std::map<std::string, Program::Uniform> uniforms;
	Program* program;

	void render_gui_segment();
	void use();
};

