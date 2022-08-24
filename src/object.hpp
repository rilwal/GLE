#pragma once

// An object represents an object in the world
// It should have various transformation properties
// And associated shaders (materials?), models, etc.
// As necessary

// In the future, maybe consider using DOP style SOA rather than AOS format for this data

#include <string>

#include <glm/glm.hpp>

#include "shaders.hpp"
#include "model.hpp"
#include "camera.hpp"

struct Object {
	std::string name;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Program* program; // TODO: create a material type that can have different uniform values for the same program
	Model* model;

	Object(std::string name);

	void render_gui();
	void render(Camera& c); // TODO: seperate renderer from object in future
};
