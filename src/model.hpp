#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct Model {
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<uint32_t> indices;

	void load_from_file(std::string filename);
};
