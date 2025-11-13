#pragma once
#include "Core/Device/DXDevice.h"
#include "Utility/LeakChecker.h"

class SHEngine {
public:

	SHEngine();
	~SHEngine();

	void Initialize();
	void Update();

private:
	//LeakChecker
	D3DResourceLeakChecker leakChecker_;

private:

	std::unique_ptr<DXDevice> dxDevice_;

};
