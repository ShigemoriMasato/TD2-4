#pragma once
#include <Core/DXDevice.h>
#include <Utility/Vector.h>

class TextureData;


class Display {
public:

	static void StaticInitialize(DXDevice* device);

	Display() = default;
	~Display() = default;

	void Initialize(TextureData* data, uint32_t clearColor);
	void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear);

	void EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);

	ID3D12Resource* GetTextureResource() const { return textureResource_; }
	RTVHandle GetRTVHandle() const { return rtvHandle_; }

private:

	static DXDevice* device_;

private:

	RTVHandle rtvHandle_;
	DSVHandle dsvHandle_;

	D3D12_RESOURCE_STATES resourceState_;
	
	ID3D12Resource* textureResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	int width_;
	int height_;
};
