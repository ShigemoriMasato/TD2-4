#pragma once
#include "Display/SwapChain.h"
#include "Display/WindowsApp.h"
#include <Core/CmdListManager.h>

struct WindowConfig {
	std::wstring windowName = L"SHEngine";
	std::wstring windowClassName = L"SHEngineClass";
	int32_t clientWidth = 1280;
	int32_t clientHeight = 720;

	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
		};
};

class Window {
public:

	Window() = default;
	~Window() = default;

	void StaticInitialize(CmdListManager* manager) {
		cmdObject_ = manager->GetCommandObject(CmdListType::SwapChain);
	}
	void Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, const WindowConfig& config, uint32_t clearColor);

	//描画可能状態にする
	void PreDraw(bool isClear);
	//テクスチャとして使用可能な状態にする
	void ToTexture();
	//Presentして、テクスチャを切り替える
	void PostDraw();

private:

	static CommandObject* cmdObject_;

	std::unique_ptr<WindowsApp> windowApp_ = nullptr;
	std::unique_ptr<SwapChain> swapChain_ = nullptr;

};
