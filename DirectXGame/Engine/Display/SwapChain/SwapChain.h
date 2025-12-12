#pragma once
#include <Core/DXDevice.h>
#include <Assets/Texture/TextureManager.h>
#include <Display/WindowsApp/WindowsApp.h>
#include <Display/Display.h>

class SwapChain {
public:

	void Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, WindowsApp* window);



private:

	Logger logger_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
};
