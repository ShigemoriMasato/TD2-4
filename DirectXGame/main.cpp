#include <Engine/SHEngine.h>
#include <Engine/Core/Display/Display.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	auto engine = std::make_unique<SHEngine>();
	engine->Initialize();

	auto display = std::make_unique<Display>();
	display->SetWindowName(L"SH-Engine");
	display->SetWindowProc([&](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {

		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
		});

	display->Create();
	display->Show(Display::kShow);

	while (engine->IsLoop()) {
		engine->Update();
	}

	return 0;
}