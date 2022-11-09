#pragma once

// A basic implementation of an asset manager.

#include <vector>
#include <map>
#include <string_view>
#include <concepts>


class IAsset {
public:
	bool _is_loaded = false;
	std::string path;
	std::string asset_type;
	IAsset(std::string _asset_type) : asset_type(_asset_type) {};


	virtual void load(const char* filename) = 0;
	virtual void unload() = 0;

	virtual void render_asset_list_entry();

	inline bool is_loaded() { return _is_loaded; };

};


template <typename T>
concept Asset = std::derived_from<T, IAsset>;


class AssetManager {
private:
	std::vector<IAsset*> assets;

public:
	template <Asset T>
	T* LoadAsset(const char * filename) {
		T* asset = new T();
		asset->load(filename);
		assets.push_back(asset);
		return asset;
	}


	void render_asset_list();

};

