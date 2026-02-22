#include "InitializeScene.h"
#include <imgui/imgui.h>
#include <Game/GameScene.h>
#include <ShigeScene/ShigeScene.h>
#include <Yokoyama/YokoyamaScene.h>

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

void InitializeScene::Initialize() {
	auto& windowSet = commonData_->mainWindow;
	windowSet.first = std::make_unique<SHEngine::Screen::WindowsAPI>();
	SHEngine::Screen::WindowsAPI::WindowDesc desc;
	desc.width = 1280;
	desc.height = 720;
	desc.windowName = L"SHEngine";
	desc.wndProc = [&](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {

#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
			return true;
		}
#endif

		return DefWindowProc(hwnd, msg, wparam, lparam);
		};

	windowSet.first->Initialize(desc, engine_->GetHInstance());
	windowSet.second = engine_->MakeWindow(windowSet.first.get(), 0xffffffff);

	input_->SetWindow(windowSet.first->GetHwnd());

	commonData_->display = std::make_unique<MainDisplay>();
	commonData_->display->Initialize(desc.width, desc.height, 0x050505FF, textureManager_, input_);

	commonData_->cmdObject = engine_->CreateCommandObject(SHEngine::Command::Type::Direct);
	engine_->ImGuiActivate(windowSet.first.get(), commonData_->cmdObject.get());

	std::vector<VertexData> vertices = {
		{{-1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{3.0f, 1.0f, 0.0f, 1.0f}, {2.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
		{{-1.0f, -3.0f, 0.0f, 1.0f}, {0.0f, 2.0f}, {0.0f, 0.0f, -1.0f}}
	};
	drawDataManager_->AddVertexBuffer(vertices);
	commonData_->postEffectDrawDataIndex = drawDataManager_->CreateDrawData();


	auto fpsObs = engine_->GetFPSObserver();
	fpsObs->SetIsFix(false);
	fpsObs->SetTargetFPS(60.0);

	//KeyManager
#pragma region 長いので折りたたみ
	commonData_->keyManager = std::make_unique<KeyManager>();

	auto keyManager = commonData_->keyManager.get();
	keyManager->Initialize(input_, commonData_->display.get());

	//KeyConfigの設定
	keyManager->SetKey(Key::Right, DIK_D, KeyState::Hold);
	keyManager->SetKey(Key::Right, DIK_RIGHTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Right, XBoxController::kRight, KeyState::Hold);
	keyManager->SetStick(Key::Right, true, false, 0.5f);

	keyManager->SetKey(Key::Left, DIK_A, KeyState::Hold);
	keyManager->SetKey(Key::Left, DIK_LEFTARROW, KeyState::Hold);
	keyManager->SetButton(Key::Left, XBoxController::kLeft, KeyState::Hold);
	keyManager->SetStick(Key::Left, true, false, -0.5f);

	keyManager->SetKey(Key::Up, DIK_W, KeyState::Hold);
	keyManager->SetKey(Key::Up, DIK_UPARROW, KeyState::Hold);
	keyManager->SetButton(Key::Up, XBoxController::kUp, KeyState::Hold);
	keyManager->SetStick(Key::Up, true, true, 0.5f);

	keyManager->SetKey(Key::Down, DIK_S, KeyState::Hold);
	keyManager->SetKey(Key::Down, DIK_DOWNARROW, KeyState::Hold);
	keyManager->SetButton(Key::Down, XBoxController::kDown, KeyState::Hold);
	keyManager->SetStick(Key::Down, true, true, -0.5f);

	//================================================================================

	keyManager->SetKey(Key::Correct, DIK_RETURN, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_SPACE, KeyState::Trigger);
	keyManager->SetKey(Key::Correct, DIK_Z, KeyState::Trigger);
	keyManager->SetButton(Key::Correct, XBoxController::kA, KeyState::Trigger);

	keyManager->SetKey(Key::Reverse, DIK_X, KeyState::Trigger);
	keyManager->SetButton(Key::Reverse, XBoxController::kB, KeyState::Trigger);

	keyManager->SetKey(Key::Pause, DIK_ESCAPE, KeyState::Trigger);
	keyManager->SetKey(Key::Pause, DIK_F1, KeyState::Trigger);
	keyManager->SetButton(Key::Pause, XBoxController::kStart, KeyState::Trigger);

	//================================================================================

	keyManager->SetKey(Key::Restart, DIK_R, KeyState::Trigger);
	keyManager->SetKey(Key::Restart, DIK_ESCAPE, KeyState::Trigger);
	keyManager->SetButton(Key::Restart, XBoxController::kSelect, KeyState::Trigger);

	keyManager->SetKey(Key::Debug1, DIK_F1, KeyState::Trigger);
	keyManager->SetKey(Key::Debug2, DIK_F2, KeyState::Trigger);
	keyManager->SetKey(Key::Debug3, DIK_F3, KeyState::Trigger);
#pragma endregion
}

std::unique_ptr<IScene> InitializeScene::Update() {
	//更新処理
	commonData_->cmdObject->ResetCommandList();

	return std::make_unique<ShigeScene>();
}

void InitializeScene::Draw() {
	auto swapChain = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	swapChain->PreDraw(cmdObj);

	engine_->DrawImGui();
	swapChain->PostDraw(cmdObj);
}
