#pragma once
#include <Screen/WindowMaker.h>

class GameWindow {
public:
	GameWindow() = default;
	~GameWindow() = default;
	void Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor);

	void PreDraw();
	void PostDraw();

	void AddDisplay(int textureIndex, std::string name, uint32_t width, uint32_t height);

	void SetState(WindowsApp::ShowType state);

	Window* GetWindow() { return window_.get(); }

private:

	struct DispConfig {
		int textureIndex;
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Window> window_;
	std::vector<DispConfig> displayTextureIndices_{};
};
