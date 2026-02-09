#pragma once
#include <Screen/WindowMaker.h>
#include <Screen/DualDisplay.h>
#include <Render/RenderObject.h>

class GameWindow {
public:
	GameWindow() = default;
	~GameWindow() = default;
	void Initialize(SHEngine* engine, const WindowConfig& config, uint32_t clearColor);

	void PreDraw(bool isClear);
	void DrawDisplayWithImGui();

	void AddDisplay(IDisplay* display, std::string name, uint32_t width, uint32_t height, bool isMain);

	void SetState(WindowsApp::ShowType state);

	Window* GetWindow() { return window_.get(); }
	Display* GetDualDisplay() { return window_->GetDisplay(); };
	CommandObject* GetCommandObject() { return window_->GetCommandObject(); }

private:

	struct DispConfig {
		IDisplay* display = nullptr;
		std::string name;
		uint32_t width;
		uint32_t height;
		bool isMain = false;
	};

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Window> window_;
	std::vector<DispConfig> displayTextureIndices_{};
	std::unique_ptr<RenderObject> renderObject_ = nullptr;

	struct DisplayInfo {
		Vector2 cursorPos;
		bool isHovered = false;
	} mainInfo_{};
};
