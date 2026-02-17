
#include <Utility/DirectUtilFuncs.h>
#include <Core/DXDevice.h>
#include <Core/Command/CommandManager.h>

#include <Render/Screen/WindowsAPI.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <Render/Screen/SwapChain.h>
#include <Render/RenderObject.h>

#include <chrono>
#include <numeric>

int WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR str, int state) {
	SHEngine::D3DResourceLeakChecker checker;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr) && "Failed to initialize COM library");

	auto device = std::make_unique<SHEngine::DXDevice>();
	device->Initialize();
	
	auto cmdManager = std::make_unique<SHEngine::Command::Manager>();
	cmdManager->Initialize(device.get());
	auto cmdObject = cmdManager->CreateCommandObject(SHEngine::Command::Type::Direct);
	auto textureManager = std::make_unique<SHEngine::TextureManager>();
	textureManager->Initialize(device.get(), cmdManager.get());

	auto drawDataManager = std::make_unique<SHEngine::DrawDataManager>();
	drawDataManager->Initialize(device.get());

	auto modelManager = std::make_unique<SHEngine::ModelManager>();
	modelManager->Initialize(textureManager.get(), drawDataManager.get());

	//! 忘れないように
	SHEngine::Screen::IDisplay::SetDevice(device.get());
	SHEngine::RenderObject::StaticInitialize(device.get());
	
	// Window作成
	auto windowsAPI = std::make_unique<SHEngine::WindowsAPI>();
	SHEngine::WindowsAPI::WindowDesc desc{};
	desc.width = 1280;
	desc.height = 720;
	desc.windowName = L"DirectXGame";
	windowsAPI->Initialize(desc, hInstance);

	// SwapChain作成
	auto swapChain = std::make_unique<SHEngine::Screen::SwapChain>();
	swapChain->Initialize(device.get(), textureManager.get(), cmdManager.get(), windowsAPI.get(), 0xff0000ff);

	auto logger = getLogger("Test");

	std::vector<double> frameTimes;
	MSG msg{};

	auto renderObject = std::make_unique<SHEngine::RenderObject>();
	renderObject->Initialize();
	renderObject->psoConfig_.isSwapChain = true;
	renderObject->psoConfig_.vs = "Simple.VS.hlsl";
	renderObject->psoConfig_.ps = "White.PS.hlsl";
	auto drawData = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);
	renderObject->SetDrawData(drawData);

	auto frameStart = std::chrono::high_resolution_clock::now();

	while(true) {

		//初期化処理
		while (msg.message != WM_QUIT) {
			//メッセージがあれば処理する
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				break;
			}
		}
		cmdObject->ResetCommandList();




		auto cmdList = cmdObject->GetCommandList();
		//描画処理
		swapChain->PreDraw(cmdObject.get(), true);

		renderObject->Draw(cmdObject.get());

		swapChain->PostDraw(cmdObject.get());

		//終了処理
		//Texture
		textureManager->UploadResources();

		//描画処理
		cmdManager->Execute(SHEngine::Command::Type::Direct);

		swapChain->Present();

		cmdManager->SendSignal(SHEngine::Command::Type::Direct);


		auto frameEnd = std::chrono::high_resolution_clock::now();
		frameTimes.push_back(std::chrono::duration<double, std::milli>(frameEnd - frameStart).count());
		frameStart = frameEnd;

		if (windowsAPI->IsCloseButtonPushed()) {
			break;
		}
	}

	// 統計情報
	double totalTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0);
	double avgTime = totalTime / frameTimes.size();

	logger->info("=== Performance Summary ===");
	logger->info("Total Frames : {}", int(frameTimes.size()));
	logger->info("Total Time   : {:.3f} ms", totalTime);
	logger->info("Average Frame: {:.3f} ms", avgTime);

	CoUninitialize();

	return 0;
}
