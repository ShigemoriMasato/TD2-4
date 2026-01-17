#pragma once
#include <Core/DXDevice.h>
#include <Assets/Texture/TextureManager.h>
#include <Screen/Display/WindowsApp.h>
#include <Screen/Display/Display.h>

class SwapChain {
public:

	void Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, WindowsApp* window, uint32_t clearColor);

	void PreDraw(CommandObject* cmdObject, bool isClear);
	void ToTexture(CommandObject* cmdObject);
	void PostDraw(CommandObject* cmdObject);
	void Present();

	Display* GetCurrentDisplay() const { return displays_[currentBackBufferIndex_].get(); }

private:

	int currentBackBufferIndex_ = 0;

	static inline int nextDebugID_ = 0;
	int debugID_ = nextDebugID_++;

	Logger logger_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	std::array<std::unique_ptr<Display>, 2> displays_{};
};
