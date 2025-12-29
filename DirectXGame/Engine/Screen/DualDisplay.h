#pragma once
#include <Core/DXDevice.h>
#include <Assets/Texture/TextureManager.h>

class DualDisplay {
public:

	static void StaticInitialize(DXDevice* device);

	DualDisplay() = default;
	~DualDisplay() = default;

	void Initialize(TextureData* data, TextureData* data2);
	void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear);

	void PostDraw(ID3D12GraphicsCommandList* commandList);

	ID3D12Resource* GetTextureResource() const { return Displays_[index_].textureResource_; }
	RTVHandle GetRTVHandle() const { return Displays_[index_].rtvHandle_; }

private:

	void EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState);

private:

	static DXDevice* device_;

private:

	struct DisplayData {
		RTVHandle rtvHandle_;
		DSVHandle dsvHandle_;

		D3D12_RESOURCE_STATES resourceState_;

		ID3D12Resource* textureResource_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	};

	DisplayData Displays_[2];
	int index_ = 0;

	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	int width_;
	int height_;

};
