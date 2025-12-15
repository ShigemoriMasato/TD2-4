#include <Core/Terminal.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	auto terminal = std::make_unique<Terminal>();
	terminal->Initialize();

	terminal->Run();

	return 0;
}