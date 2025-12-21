#include "GameWindow.h"
#include <SHEngine.h>
#include <imgui/imgui.h>

void GameWindow::Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor) {
	window_ = engine->GetWindowMaker()->MakeWindow(config, clearColor);
	textureManager_ = engine->GetTextureManager();
}

void GameWindow::PreDraw() {
	window_->PreDraw(true);
}

void GameWindow::PostDraw() {

	//todo ImGuiの描画とか
	for(const auto& config : displayTextureIndices_) {
		ImGui::Begin(config.name.c_str());
		TextureData* textureData = textureManager_->GetTextureData(config.textureIndex);
		auto textureResource = textureManager_->GetTextureResource(config.textureIndex);
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Image(
			reinterpret_cast<ImTextureID>(textureResource.Get()),
			ImVec2(static_cast<float>(config.width), static_cast<float>(config.height))
		);
		ImGui::End();
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

void GameWindow::SetState(WindowsApp::ShowType state) {
}
