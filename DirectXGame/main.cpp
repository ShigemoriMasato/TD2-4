#include <SHEngine.h>

int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	auto engine = std::make_unique<SHEngine>();
	engine->Initialize();

	int* a = nullptr;
	*a = 0;

	while (engine->IsLoop()) {

		//更新===
		engine->Update();


		//描画===
		engine->PreDraw();

		engine->EndFrame();
	}

	return 0;
}