#pragma once
#include <Screen/WindowMaker.h>
#include <Screen/DualDisplay.h>
#include <Render/RenderObject.h>

class GameWindow {
public:
	GameWindow() = default;
	~GameWindow() = default;
	void Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor);

	void PreDraw();
	void PostDraw();

	void AddDisplay(int textureIndex, std::string name, uint32_t width, uint32_t height);
	void AddDisplay(DualDisplay* display, std::string name, uint32_t width, uint32_t height);

	void SetState(WindowsApp::ShowType state);

	Window* GetWindow() { return window_.get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return window_->GetCommandObject()->GetCommandList(); }

private:

	struct DispConfig {
		int textureIndex;
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	struct DualDispConfig {
		DualDisplay* display;
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Window> window_;
	std::vector<DispConfig> displayTextureIndices_{};
	std::vector<DualDispConfig> dualDisplayTextureIndices_{};
	std::unique_ptr<RenderObject> renderObject_ = nullptr;
};
