#pragma once
#include <Core/DXDevice.h>
#include <Core/CmdListManager.h>
#include <Screen/Window.h>

class ImGuiforEngine {
public:

	void Initialize(DXDevice* device, CmdListManager* cmdManager, Window* window);

	void BeginFrame();
	void EndFrame();

	void Finalize();

private:

	Logger logger_ = nullptr;

	SRVManager* srv_;

	SRVHandle srvHandle_;
	SRVHandle srvHandle2_;
	CommandObject* commandObject_ = nullptr;

};

