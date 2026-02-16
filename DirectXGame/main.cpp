#include <Core/DXDevice.h>
#include <Core/Command/CommandManager.h>

int WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR str, int state) {

	auto device = std::make_unique<SHEngine::DXDevice>();
	device->Initialize();
	auto cmdManager = std::make_unique<SHEngine::Command::Manager>();
	cmdManager->Initialize(device.get());
	
	return 0;
}