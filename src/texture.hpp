#pragma once

#include "asset_manager.hpp"

class Texture : public  IAsset {
protected:
	int32_t width = 0;
	int32_t height = 0;
	int32_t channels = 0;

	uint32_t gl_tex_id;

	uint8_t* data = nullptr;
public:
	Texture() : IAsset("Texture") {};
	virtual void load(const char* filename) = 0;
	virtual void unload() = 0;
	virtual void render_asset_list_entry() = 0;

	inline uint32_t get_tex_id() { return gl_tex_id; }

};


class Texture2D : public Texture {
public:
	Texture2D() : Texture() {};
	void load(const char* filename) override;
	void unload() override;
	void render_asset_list_entry() override;
};

class TextureCube : public Texture {
public:
	TextureCube() : Texture() {};
	void load(const char* filename) override;
	void unload() override;
	void render_asset_list_entry() override;
};
