#pragma once
#include <Core/DXDevice.h>
#include <Utility/Vector.h>
#include <Assets/Texture/TextureData.h>

class IDisplay {
public:
	virtual ~IDisplay() = default;

	virtual void PreDraw(ID3D12GraphicsCommandList* commandList, bool isClear) = 0;

	virtual void ToTexture(ID3D12GraphicsCommandList* commandList) = 0;
	virtual void PostDraw(ID3D12GraphicsCommandList* commandList) = 0;

	virtual ID3D12Resource* GetTextureResource() const = 0;
	virtual RTVHandle GetRTVHandle() const = 0;
	virtual TextureData* GetTextureData() const = 0;

};
