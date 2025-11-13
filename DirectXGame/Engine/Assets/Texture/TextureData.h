#pragma once
#include <string>
#include <d3d12.h>
#include <Core/View/SRVManager.h>

using namespace Microsoft::WRL;

class TextureData {
public:

	TextureData() = default;
	~TextureData() = default;

	//Window用のテクスチャを作成
	void Create(uint32_t width, uint32_t height);

private:

	friend class TextureManager;
	void Create(std::string filePath, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, SRVManager* srvManager);

private:

	uint32_t width_;
	uint32_t height_;

	ComPtr<ID3D12Resource> textureResource_ = nullptr;

	//CommandListが一度executeされたら削除するやつ
	Microsoft::WRL::ComPtr<ID3D12Resource> intermadiateResource_ = nullptr;

	SRVHandle srvHandle_;

};

