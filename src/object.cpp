
#include "object.hpp"

#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include "property_list.hpp"

Object::Object(std::string name) {
	this->name = name;
	program = nullptr;
	model = nullptr;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 1, 1, 1 };
}


void Object::render_gui() {
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
			program->render_gui_segment();

			ImGui::EndPropListEntry();
		}

		ImGui::EndPropList();


	}

	ImGui::End();
	ImGui::PopID();

}


void Object::render(Camera& c) {
	if (!program) return;
	if (!model) return;

	glm::mat4 translation_matrix = glm::translate(position);
	glm::mat4 rotation_matrix = glm::rotate(glm::rotate(glm::rotate(rotation.x, glm::vec3{0, 1, 0}), rotation.y, { 1.f, 0.f, 0.f }), rotation.z, { 0.f, 0.f, 1.f });
	glm::mat4 scale_matrix = glm::scale(scale);

	glm::mat4 model_matrix = translation_matrix * rotation_matrix * scale_matrix;

	glm::mat4 mvp = c.vp * model_matrix;

	// TODO: remember the position of these fixed matrices, rather than searching every render
	for (auto& [name, uniform] : program->uniforms) {
		if (name == "_mvp")
			uniform.value.m4 = mvp;
		if (name == "_model")
			uniform.value.m4 = model_matrix;
	}
	
	program->use();
	
	// TODO: move this responsibility elsewhere
	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, &model->indices[0]);

}