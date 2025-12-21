#include "../InitializeScene.h"
#include <Test/TestScene.h>
#include <imgui/imgui.h>

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

void InitializeScene::Initialize() {
	{
		WindowConfig config;
		config.windowName = L"DirectXGame";
		config.windowProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {

#ifdef USE_IMGUI
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
				return true;
			}
#endif

			switch (msg) {
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			}
			return DefWindowProc(hwnd, msg, wparam, lparam);
			};

		config.clientWidth = 1280;
		config.clientHeight = 720;

		uint32_t clearColor = 0x0060b0ff;

		commonData_->mainWindow = std::make_unique<GameWindow>();
		commonData_->mainWindow->Initialize(engine_, config, clearColor);
	}

	engine_->ImGuiActivate(commonData_->mainWindow->GetWindow());

	{
		auto& display = commonData_->display;
		display = std::make_unique<Display>();

		uint32_t clearColor = 0xff8080ff;

		int textureIndex = textureManager_->CreateWindowTexture(1280, 720, clearColor);
		auto textureData = textureManager_->GetTextureData(textureIndex);
		display->Initialize(textureData, 0xff000000);

		//commonData_->mainWindow->AddDisplay(textureIndex, "Main Display", 1280 / 2, 720 / 2);
	}

	//todo リソースの全読み込み
}

std::unique_ptr<IScene> InitializeScene::Update() {
	return std::make_unique<TestScene>();
}

void InitializeScene::Draw() {
}
