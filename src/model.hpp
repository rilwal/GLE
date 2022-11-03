#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "asset_manager.hpp"
#include "VertexBuffer.hpp"

struct Model : public IAsset {
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<uint32_t> indices;

	std::vector<uint8_t> gl_data;

	VertexArrayLayout layout;

	Model() : IAsset("Model") {};

	void load(const char* filename) override;
	void unload() override;
};
