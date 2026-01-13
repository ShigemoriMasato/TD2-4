#include "GameWindow.h"
#include <SHEngine.h>
#include <imgui/imgui.h>

void GameWindow::Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor) {
	window_ = engine->GetWindowMaker()->MakeWindow(config, clearColor);
	textureManager_ = engine->GetTextureManager();

#ifdef SH_RELEASE

#endif
}

void GameWindow::PreDraw() {
	window_->PreDraw(true);
}

void GameWindow::DrawDisplayWithImGui() {

#ifdef USE_IMGUI

	for(const auto& config : displayTextureIndices_) {
		ImGui::Begin(config.name.c_str());
		TextureData* textureData = textureManager_->GetTextureData(config.textureIndex);
		auto textureHandle = textureData->GetGPUHandle();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Image(
			ImTextureID(textureHandle.ptr),
			ImVec2(static_cast<float>(config.width), static_cast<float>(config.height))
		);
		ImGui::End();
	}

	for(const auto& config : dualDisplayTextureIndices_) {
		ImGui::Begin(config.name.c_str());
		auto textureHandle = config.display->GetTextureResource();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Image(
			ImTextureID(textureHandle),
			ImVec2(static_cast<float>(config.width), static_cast<float>(config.height))
		);
		ImGui::End();
	}

	return;

#endif

	if (dualDisplayTextureIndices_.size() < 1) {
		return;
	}

	for (const auto& config : dualDisplayTextureIndices_) {

	}

}

void GameWindow::AddDisplay(int textureIndex, std::string name, uint32_t width, uint32_t height) {
	DispConfig config;
	config.textureIndex = textureIndex;
	config.name = name;
	config.width = width;
	config.height = height;
	displayTextureIndices_.push_back(config);
}

void GameWindow::AddDisplay(DualDisplay* display, std::string name, uint32_t width, uint32_t height) {
	DualDispConfig config;
	config.display = display;
	config.name = name;
	config.width = width;
	config.height = height;
	dualDisplayTextureIndices_.push_back(config);
}

void GameWindow::SetState(WindowsApp::ShowType state) {
}
