#pragma once
#include "Core/DXDevice.h"
#include <Utility/LeakChecker.h>
#include <Core/SRVManager.h>
#include <Core/RTVManager.h>
#include <Core/DSVManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Core/CmdListManager.h>
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

private:

	void ExecuteMessage();

private:

	//LeakChecker
	D3DResourceLeakChecker leakChecker_;

	std::unique_ptr<SRVManager> srvManager_;
	std::unique_ptr<RTVManager> rtvManager_;
	std::unique_ptr<DSVManager> dsvManager_;

private:

	std::unique_ptr<DXDevice> dxDevice_;
	std::unique_ptr<CmdListManager> cmdListManager_;

	std::unique_ptr<TextureManager> textureManager_;

private:

	bool exit_ = false;
	bool pushedCrossButton_ = false;

	MSG msg_;

};
