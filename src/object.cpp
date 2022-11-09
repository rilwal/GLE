
#include "object.hpp"

#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include "property_list.hpp"

#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "shaders.hpp"

float radians(float degrees) {
	return degrees * (M_PI / 180.f);
}

Object::Object(std::string name) {
	this->name = name;
	program = nullptr;
	model = nullptr;
	material_id = 0;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 1, 1, 1 };
}


void Object::render_gui() {
	Material& material = program->materials[material_id];

	ImGui::PushID(this);
	if (ImGui::Begin(name.c_str())) {
		if (ImGui::BeginPropList("Train")) {

			if (ImGui::BeginPropListEntry("Transform")) {
				ImGui::TableNextRow();
				ImGui::PLProp("Position", position);
				ImGui::PLProp("Rotation", rotation);
				ImGui::PLProp("Scale", scale);

				ImGui::EndPropListEntry();
			}

		}

		if (ImGui::BeginPropListEntry("Material")) {
			material.render_gui_segment();

			ImGui::EndPropListEntry();
		}

		ImGui::EndPropList();

	}

	ImGui::End();
	ImGui::PopID();

}


void Object::render(Camera& c) {
	vertex_buffer->bind();
	Material& material = program->materials[material_id];

	if (!program) return;
	if (!model) return;

	glm::mat4 translation_matrix = glm::translate(position);
	glm::mat4 rotation_matrix = glm::rotate(glm::rotate(glm::rotate(radians(rotation.x), glm::vec3{ 1, 0, 0 }), radians(rotation.y), { 0.f, 1.f, 0.f }), radians(rotation.z), { 0.f, 0.f, 1.f });
	glm::mat4 scale_matrix = glm::scale(scale);

	glm::mat4 model_matrix = translation_matrix * rotation_matrix * scale_matrix;


	// set up fixed name uniforms
	if (material.uniforms.contains("_mvp")) {
		glm::mat4 mvp = c.vp * model_matrix;
		material.uniforms["_mvp"].value.m4 = mvp;
	}

	if (material.uniforms.contains("_model")) {
		material.uniforms["_model"].value.m4 = model_matrix;
	}



	if (material.uniforms.contains("_time")) {
		float time = glfwGetTime();
		material.uniforms["_time"].value.f = time;
	}



	material.use();
	
	// TODO: move this responsibility elsewhere
	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, &model->indices[0]);
}

