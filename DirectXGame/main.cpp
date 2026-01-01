#include <Core/Terminal.h>

int WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR str, int state) {
	auto terminal = std::make_unique<Terminal>();
	terminal->Initialize(hInstance);
	terminal->Run();
	return 0;
}