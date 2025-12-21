#pragma once
#include <Core/DXDevice.h>
#include <Core/CmdListManager.h>
#include <Assets/Texture/TextureManager.h>
#include "Window.h"

class SHEngine;

class WindowMaker {
public:

	void Initialize(DXDevice* device, TextureManager* textureManager, CmdListManager* cmdListManager);

	std::unique_ptr<Window> MakeWindow(const WindowConfig& config, uint32_t clearColor);

private:

	friend class SHEngine;
	void AllPostDraw();
	void PresentAllWindows();

private:

	DXDevice* device_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	CmdListManager* cmdListManager_ = nullptr;

	std::vector<Window*> windows_{};

};
