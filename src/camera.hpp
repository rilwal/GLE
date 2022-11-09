#pragma once

// Basic implementation of camera.
// For now directly contains VP matrix.
// In the future, make this smart


#include <glm/glm.hpp>


struct Camera {
	glm::mat4 vp;
	glm::mat4 v;
	glm::mat4 p;

	float gamma = 1.0;
	float exposure = 2.0;
};

