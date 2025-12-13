#pragma once
#include "TextureData.h"
#include <Core/DXDevice.h>
#include <map>
#include <memory>

class TextureManager {
public:

	TextureManager() = default;
	~TextureManager() = default;

	void Initialize(DXDevice* device);

	int LoadTexture(const std::string& filePath);
	int CreateWindowTexture(uint32_t width, uint32_t height, uint32_t clearColor);
	int CreateSwapChainTexture(ID3D12Resource* resource);

	TextureData* GetTextureData(int handle);
	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(int handle) {
		return textureDatas_[handle]->textureResource_;
	}

	void UploadTextures(ID3D12GraphicsCommandList* cmdList);
	void ClearUploadedResources();

private:

	void CheckMaxCount(int offset);

	DXDevice* device_ = nullptr;
	SRVManager* srvManager_ = nullptr;

	const int maxTextureCount = 512;

	std::map<int, std::unique_ptr<TextureData>> textureDatas_;

	std::vector< std::pair<ID3D12Resource*, DirectX::ScratchImage>> uploadResources_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

};

