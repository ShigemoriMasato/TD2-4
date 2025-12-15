#pragma once
#include <Core/DXDevice.h>
#include <Utility/LeakChecker.h>
#include <Screen/Window.h>
#include <Core/FenceManager.h>
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

	std::unique_ptr<Window> MakeWindow(const WindowConfig& config, uint32_t clearColor);

private:

	void ExecuteMessage();

private:

	//LeakChecker
	D3DResourceLeakChecker leakChecker_{};

	//最後に削除する
	std::unique_ptr<DXDevice> dxDevice_ = nullptr;

private:

	std::unique_ptr<CmdListManager> cmdListManager_ = nullptr;
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
	std::unique_ptr<FenceManager> fenceManager_ = nullptr;

private:

	std::vector<Window*> windows_{};

	bool exit_ = false;
	bool pushedCrossButton_ = false;

	MSG msg_{};

};
