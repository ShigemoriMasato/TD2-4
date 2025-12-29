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
		display = std::make_unique<DualDisplay>();

		uint32_t clearColor = 0xff8080ff;

		int textureIndex = textureManager_->CreateWindowTexture(1280, 720, clearColor);
		int textureIndex2 = textureManager_->CreateWindowTexture(1280, 720, clearColor);
		auto textureData = textureManager_->GetTextureData(textureIndex);
		auto textureData2 = textureManager_->GetTextureData(textureIndex2);
		display->Initialize(textureData, textureData2);

		commonData_->mainWindow->AddDisplay(textureIndex, "Main Display", 1280 / 2, 720 / 2);
	}

	//todo リソースの全読み込み
}

std::unique_ptr<IScene> InitializeScene::Update() {
	return std::make_unique<TestScene>();
}

void InitializeScene::Draw() {
	commonData_->display->PreDraw(commonData_->mainWindow->GetCommandList(), true);
	commonData_->display->PostDraw(commonData_->mainWindow->GetCommandList());
	commonData_->mainWindow->PreDraw();
	commonData_->mainWindow->PostDraw();
}
