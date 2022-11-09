#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <imgui.h>

#include "property_list.hpp"

void Texture2D::load(const char* filename)
{
	path = filename;
	data = stbi_load(filename, &width, &height, &channels, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &gl_tex_id);
	glBindTexture(GL_TEXTURE_2D, gl_tex_id);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	//TODO: consider freeing our image data here?
	// However we may need to stream on and off the gpu to manage VRAM
	// And also on and off disk to save system RAM??
	// This really needs to be more thought out
}

void Texture2D::unload()
{
	stbi_image_free(data);

	glDeleteTextures(1, &gl_tex_id);
}

void Texture2D::render_asset_list_entry()
{
	bool open = ImGui::BeginPropListEntry(path);

	ImGui::TableNextRow();
	if (open) {
		ImGui::PLInfo("Type", "Texture");
		ImGui::PLInfo("Width", width);
		ImGui::PLInfo("Height", height);
		ImGui::PLProp("Preview", *this);
		//ImGui::Image((ImTextureID)gl_tex_id, ImVec2(10.0, 10.0), ImVec2(0, 1), ImVec2(1, 0));


		ImGui::EndPropListEntry();
	}

	ImGui::PopID();

}



void TextureCube::load(const char* filename)
{
	path = filename;

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &gl_tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gl_tex_id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++) {
		// load all 6 faces
		std::string face_filename = std::format("{}.{}.png", filename, i);
		printf(std::format("Loading from {}\n", face_filename).c_str());
		data = stbi_load(face_filename.c_str(), &width, &height, &channels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}


	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//TODO: consider freeing our image data here?
	// However we may need to stream on and off the gpu to manage VRAM
	// And also on and off disk to save system RAM??
	// This really needs to be more thought out
}

void TextureCube::unload()
{
	stbi_image_free(data);

	glDeleteTextures(1, &gl_tex_id);
}

void TextureCube::render_asset_list_entry()
{
	bool open = ImGui::BeginPropListEntry(path);

	ImGui::TableNextRow();
	if (open) {
		ImGui::PLInfo("Type", "Texture");
		ImGui::PLInfo("Width", width);
		ImGui::PLInfo("Height", height);
		ImGui::PLProp("Preview", *this);
		//ImGui::Image((ImTextureID)gl_tex_id, ImVec2(10.0, 10.0), ImVec2(0, 1), ImVec2(1, 0));


		ImGui::EndPropListEntry();
	}

	ImGui::PopID();

}


