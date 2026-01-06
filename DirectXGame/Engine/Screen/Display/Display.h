#pragma once
#include "IDisplay.h"

//非推奨クラス
class Display : public IDisplay {
public:

	static void StaticInitialize(DXDevice* device);

	Display() = default;
	~Display();

	void Initialize(TextureData* data, uint32_t color);
	void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear) override;

	void ToTexture(ID3D12GraphicsCommandList* commandList) override;
	void PostDraw(ID3D12GraphicsCommandList* commandList) override;

	ID3D12Resource* GetTextureResource() const override { return textureData->GetResource(); }
	RTVHandle GetRTVHandle() const override { return rtvHandle_; }
	TextureData* GetTextureData() const override { return textureData; }

private:

	void EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState);

private:

	static DXDevice* device_;

private:

	RTVHandle rtvHandle_;
	DSVHandle dsvHandle_;

	D3D12_RESOURCE_STATES resourceState_;
	
	TextureData* textureData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	int width_;
	int height_;
};
