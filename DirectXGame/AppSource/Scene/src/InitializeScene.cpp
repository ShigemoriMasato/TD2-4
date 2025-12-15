#include "../InitializeScene.h"

void InitializeScene::Initialize() {
	WindowConfig config;
	config.windowName = L"DirectXGame";
	config.windowProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
		};

}

std::unique_ptr<IScene> InitializeScene::Update() {
	return std::unique_ptr<IScene>();
}

void InitializeScene::Draw() {
}
