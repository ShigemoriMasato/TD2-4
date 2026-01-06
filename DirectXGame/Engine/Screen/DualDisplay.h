#pragma once
#include "Display/IDisplay.h"

class DualDisplay : public IDisplay {
public:

	static void StaticInitialize(DXDevice* device);

	DualDisplay() = default;
	~DualDisplay();

	void Initialize(TextureData* data, TextureData* data2);
	void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear) override;

	void ToTexture(ID3D12GraphicsCommandList* commandList) override;
	void PostDraw(ID3D12GraphicsCommandList* commandList) override;

	ID3D12Resource* GetTextureResource() const override { return Displays_[index_].textureData_->GetResource(); }
	RTVHandle GetRTVHandle() const override { return Displays_[index_].rtvHandle_; }
	TextureData* GetTextureData() const override { return Displays_[index_].textureData_; }

private:

	void EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState);

private:

	static DXDevice* device_;

private:

	friend class Display;
	struct DisplayData {
		RTVHandle rtvHandle_;
		DSVHandle dsvHandle_;

		D3D12_RESOURCE_STATES resourceState_;

		TextureData* textureData_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	};

	DisplayData Displays_[2]{};
	int index_ = 0;

	Vector4 clearColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

	int width_{};
	int height_{};

};
