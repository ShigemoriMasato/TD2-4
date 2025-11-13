#pragma once
#include "Core/Device/DXDevice.h"
#include "Utility/LeakChecker.h"

class SHEngine {
public:

	SHEngine();
	~SHEngine();

	void Initialize();

	bool IsLoop();

	void Update();

	void PreDraw();
	void PostDraw();

private:

	//LeakChecker
	D3DResourceLeakChecker leakChecker_;

private:

	std::unique_ptr<DXDevice> dxDevice_;

	MSG msg_;

};
