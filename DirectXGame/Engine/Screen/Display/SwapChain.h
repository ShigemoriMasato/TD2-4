#pragma once
#include <Core/DXDevice.h>
#include <Assets/Texture/TextureManager.h>
#include <Screen/Display/WindowsApp.h>
#include <Screen/Display/Display.h>

class SwapChain {
public:

	void Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, WindowsApp* window, uint32_t clearColor);

	void PreDraw(ID3D12GraphicsCommandList* cmdList, bool isClear);
	void ToTexture(ID3D12GraphicsCommandList* cmdList);
	void Present();

private:

	Logger logger_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	std::array<Display, 2> displays_{};
};
