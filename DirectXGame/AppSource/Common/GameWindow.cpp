#include "GameWindow.h"
#include <SHEngine.h>
#include <imgui/imgui.h>

void GameWindow::Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor) {
	window_ = engine->GetWindowMaker()->MakeWindow(config, clearColor);
	textureManager_ = engine->GetTextureManager();
}

void GameWindow::PreDraw(bool isClear) {
	window_->PreDraw(isClear);
}

void GameWindow::DrawDisplayWithImGui() {

#ifdef USE_IMGUI

	POINT cursorPos;
	if (GetCursorPos(&cursorPos)) {
		// スクリーン座標をクライアント座標に変換
		ScreenToClient(window_->GetHwnd(), &cursorPos);
		// カーソル位置をワールド座標に変換
		mainInfo_.cursorPos = { static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y) };
	}

	for (const auto& config : displayTextureIndices_) {
		bool isOpen = ImGui::Begin(config.name.c_str());

		if (!isOpen && config.isMain) {
			mainInfo_.isHovered = false;
		}

		int textureIndex = config.display->GetTextureData()->GetOffset();
		TextureData* textureData = textureManager_->GetTextureData(textureIndex);
		auto textureHandle = textureData->GetGPUHandle();
		ImGuiIO& io = ImGui::GetIO();

		// Windowが表示される位置を取得
		Vector2 screenPos{};
		if (config.isMain) {
			ImVec2 tmp = ImGui::GetCursorScreenPos();
			screenPos.x = tmp.x;
			screenPos.y = tmp.y;
		}

		ImGui::Image(
			ImTextureID(textureHandle.ptr),
			ImVec2(static_cast<float>(config.width), static_cast<float>(config.height))
		);

		// 画像内のマウス位置を取得する
		if (config.isMain) {
			if (ImGui::IsItemHovered()) {
				mainInfo_.isHovered = true;
				mainInfo_.cursorPos -= screenPos;

				//正規化して、1280x720に戻す
				mainInfo_.cursorPos = {
					mainInfo_.cursorPos.x / static_cast<float>(config.width) * 1280,
					mainInfo_.cursorPos.y / static_cast<float>(config.height) * 720
				};

			} else {
				mainInfo_.isHovered = false;
			}
		}

		ImGui::End();
	}

	return;

#endif

}

void GameWindow::AddDisplay(IDisplay* display, std::string name, uint32_t width, uint32_t height, bool isMain) {
	DispConfig config;
	config.display = display;
	config.name = name;
	config.width = width;
	config.height = height;
	config.isMain = isMain;
	displayTextureIndices_.push_back(config);
}

void GameWindow::SetState(WindowsApp::ShowType state) {
	window_->SetState(state);
}
