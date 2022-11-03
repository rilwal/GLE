#include "asset_manager.hpp"
#include "property_list.hpp"

#include <imgui.h>

void AssetManager::render_asset_list() {
	static bool show = true;
	if (show) {
		if (ImGui::Begin("Asset List", &show)) {
			if (ImGui::BeginPropList("Assets")) {
				for (const auto& asset : assets) {
					ImGui::PushID(asset);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					bool open = ImGui::BeginPropListEntry(asset->path);

					ImGui::TableNextRow();
					if (open) {
						ImGui::PLInfo("Type", asset->asset_type);

						ImGui::EndPropListEntry();
					}

					ImGui::PopID();

				}


				ImGui::EndPropList();

			}
		}
		ImGui::End();
	}
}