#pragma once
#include <SHEngine.h>
#include <Common/GameWindow.h>

struct CommonData {
	std::unique_ptr<DualDisplay> display;
	std::unique_ptr<GameWindow> mainWindow;
};
