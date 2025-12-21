#include "../ImGuiforEngine.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>

void ImGuiforEngine::Initialize(DXDevice* device, CmdListManager* cmdManager, Window* window) {
	logger_ = LogSystem::getLogger("ImGui");
	logger_->set_level(spdlog::level::debug);

	srv_ = std::make_unique<SRVManager>(device->GetDevice(), device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 16);

#ifdef USE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window->GetHwnd());
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキングを有効化

	ImGui_ImplDX12_InitInfo initInfo;
	initInfo.Device = device->GetDevice();
	initInfo.NumFramesInFlight = 1;
	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	initInfo.CommandQueue = cmdManager->GetCommandQueue();
	initInfo.SrvDescriptorHeap = srv_->GetHeap();

	srvHandle_.UpdateHandle(srv_.get(), 0);

	initInfo.LegacySingleSrvCpuDescriptor = srvHandle_.GetCPU();
	initInfo.LegacySingleSrvGpuDescriptor = srvHandle_.GetGPU();

    ImGui_ImplDX12_Init(&initInfo);

	commandObject_ = window->GetCommandObject();

    logger_->info("ImGui Activate");

    return;
#endif

	logger_->warn("ImGui is not activated");
}

void ImGuiforEngine::BeginFrame() {
#ifdef USE_IMGUI
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	logger_->debug("ImGui Open");

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); // 角丸をなくす

    ImGui::Begin("DockSpaceWindow", nullptr, ImGuiWindowFlags_NoTitleBar | // タイトルバーなし 
        ImGuiWindowFlags_NoResize | // リサイズ不可
        ImGuiWindowFlags_NoMove | // 移動不可
        ImGuiWindowFlags_NoScrollbar | // スクロールバーなし
        ImGuiWindowFlags_NoCollapse | // 折り畳み不可
        ImGuiWindowFlags_NoBackground | // 背景なし（必要に応じて）
        ImGuiWindowFlags_NoSavedSettings);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
    ImGui::PopStyleVar(2);

#endif
}

void ImGuiforEngine::EndFrame() {
#ifdef USE_IMGUI

    // ディスクリプタヒープをコマンドリストに設定
    ID3D12DescriptorHeap* heaps[] = { srv_->GetHeap() };
    commandObject_->GetCommandList()->SetDescriptorHeaps(1, heaps);

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandObject_->GetCommandList());

    logger_->debug("ImGui Close");

#endif
}

void ImGuiforEngine::Finalize() {
#if USE_IMGUI
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif
}
