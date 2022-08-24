#pragma once

/* Define a basic property list to be used with ImGui */

#include <string>

#include <imgui.h>

#include "shaders.hpp"


// Add to the ImGui namespace for convenience
namespace ImGui {
	constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;



	// A combo box for shader types
	void ShaderTypeComboBox(std::string label, Shader::Type& prop) {
		static std::vector<const char*> labels;
		static std::vector<int> values;

		static bool initialized = false;

		if (!initialized) {
			initialized = true;
			for (const auto& pair : Shader::TypeNames) {
				labels.push_back(pair.second.c_str());
				values.push_back(static_cast<int>(pair.first));
			}
		}

		int current = std::find(values.begin(), values.end(), static_cast<int>(prop)) - values.begin();
		if (ImGui::Combo(label.c_str(), &current, &labels[0], (int)labels.size()))
			prop = static_cast<Shader::Type>(values[current]);

	}



	inline bool BeginPropList(std::string name, ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable) {
		return ImGui::BeginTable("split", 2, flags);
	}

	inline void EndPropList() {
		ImGui::EndTable();
	}

	inline bool BeginPropListEntry(std::string name) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		bool open = ImGui::TreeNode(name.c_str(), name.c_str());
		ImGui::AlignTextToFramePadding();

		return open;

	}

	inline void EndPropListEntry() {
		ImGui::TreePop();
	}

	// A string property for PropList
	inline void PLProp(std::string label, std::string& prop) {


		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::InputText("", &prop);

		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}

	// A string info property for PropList
	inline void PLInfo(std::string label, const std::string& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::Text("", prop.c_str());

		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}


	// A shader type property for PropList
	inline void PLProp(std::string label, Shader::Type &prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::ShaderTypeComboBox("", prop);

		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}


	// A float for PropList
	inline void PLProp(std::string label, float& prop) {

		constexpr auto leaf_node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::DragFloat("", &prop, .1);
		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}


	// A float with range for PropList
	inline void PLProp(std::string label, float& prop, float min, float max, float step=INFINITY) {		
		if (step == INFINITY) {
			step = (max - min) / 1000.f;
		}

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::DragFloat("", &prop, step, min, max, "%.3f");
		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}


	// A shader PropList entry
	inline void PLProp(std::string label, Shader*& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHADER")) {
				prop = *(Shader**)payload->Data;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::Text(prop ? prop->name.c_str() : "");
		ImGui::NextColumn();
		ImGui::PopID();

		ImGui::TableNextRow();
	}


	// A bool PropList entry
	inline void PLProp(std::string label, bool& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::Checkbox("", &prop);
		ImGui::NextColumn();
		ImGui::PopID();
		ImGui::TableNextRow();
	}

	// An RO bool PropList entry
	inline void PLInfo(std::string label, bool& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::Text(prop ? "True" : "False");
		ImGui::NextColumn();
		ImGui::PopID();
		ImGui::TableNextRow();
	}


	// A Vec3 entry
	inline void PLProp(std::string label, glm::vec3& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::DragFloat3("", &prop[0], .1);
		ImGui::NextColumn();
		ImGui::PopID();
		ImGui::TableNextRow();
	}

	// A color entry
	inline void PLColor(std::string label, glm::vec3& prop) {

		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);

		ImGui::ColorEdit3("", &prop[0]);
		ImGui::NextColumn();
		ImGui::PopID();
		ImGui::TableNextRow();
	}

	// A Mat4 entry
	inline void PLProp(std::string label, glm::mat4& prop) {
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();

		ImGui::TreeNodeEx(label.c_str(), leaf_node_flags, label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::PushID(&prop);

		ImGui::PushID("row_0");
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat4("", &prop[0][0], .1);
		ImGui::PopID();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		ImGui::PushID("row_1");
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat4("", &prop[1][0], .1);
		ImGui::PopID();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		ImGui::PushID("row_2");
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat4("", &prop[2][0], .1);
		ImGui::PopID();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(1);

		ImGui::PushID("row_3");
		ImGui::SetNextItemWidth(-FLT_MIN);
		ImGui::DragFloat4("", &prop[3][0], .1);
		ImGui::PopID();

		ImGui::PopID();


		ImGui::NextColumn();
		ImGui::TableNextRow();
	}
}

