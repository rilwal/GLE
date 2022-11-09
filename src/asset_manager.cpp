#include "asset_manager.hpp"
#include "property_list.hpp"

#include <imgui.h>

void IAsset::render_asset_list_entry() {
	bool open = ImGui::BeginPropListEntry(path);

	ImGui::TableNextRow();
	if (open) {
		ImGui::PLInfo("Type", "asset_type");

		ImGui::EndPropListEntry();
	}

	ImGui::PopID();
}

void AssetManager::render_asset_list() {
	static bool show = true;
	if (show) {
		if (ImGui::Begin("Asset List", &show)) {
			if (ImGui::BeginPropList("Assets")) {
				for (const auto& asset : assets) {
					ImGui::PushID(asset);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					asset->render_asset_list_entry();

				}


				ImGui::EndPropList();

			}
		}
		ImGui::End();
	}
}