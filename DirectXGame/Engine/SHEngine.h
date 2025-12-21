#pragma once
#include <Core/DXDevice.h>
#include <Utility/LeakChecker.h>
#include <Screen/Window.h>
#include <Core/FenceManager.h>
#include <Screen/WindowMaker.h>
#include <Core/ImGuiforEngine.h>
#include <memory>

class SHEngine {
public:

	SHEngine();
	~SHEngine();

	void Initialize();

	bool IsLoop();
	void Update();
	void PreDraw();
	void EndFrame();

	void ImGuiActivate(Window* window);
	void ImGuiDraw();

	TextureManager* GetTextureManager() { return textureManager_.get(); }
	WindowMaker* GetWindowMaker() { return windowMaker_.get(); }

private:

	void ExecuteMessage();

private:

	//LeakChecker
	D3DResourceLeakChecker leakChecker_{};

	//最後に削除する
	std::unique_ptr<DXDevice> dxDevice_ = nullptr;

private:

	//Engine側ツール
	std::unique_ptr<CmdListManager> cmdListManager_ = nullptr;
	std::unique_ptr<FenceManager> fenceManager_ = nullptr;
	std::unique_ptr<ImGuiforEngine> imGuiForEngine_ = nullptr;

	//App側ツール
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	std::unique_ptr<WindowMaker> windowMaker_ = nullptr;

private:

	bool exit_ = false;
	bool pushedCrossButton_ = false;

	bool imGuiActive_ = false;
	bool imguiDrawed_ = false;

	MSG msg_{};

	Logger logger_ = nullptr;
};
