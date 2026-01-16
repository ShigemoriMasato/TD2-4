#include "DualDisplay.h"
#include <Utility/DirectUtilFuncs.h>
#include <Utility/Color.h>

DXDevice* DualDisplay::device_ = nullptr;

namespace {

    ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
        //生成するResourceの設定
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = width;
        resourceDesc.Height = height;
        resourceDesc.MipLevels = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        //利用するヒープの設定
        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

        //深度値のクリア設定
        D3D12_CLEAR_VALUE depthClearValue{};
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

        //Resourceの生成
        ID3D12Resource* resource = nullptr;
        HRESULT hr = device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue,
            IID_PPV_ARGS(&resource));
        assert(SUCCEEDED(hr));

        return resource;
    }

    std::map<D3D12_RESOURCE_STATES, std::string> resourceStateToString = {
        {D3D12_RESOURCE_STATE_PRESENT, "PRESENT"},
        {D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, "VERTEX_AND_CONSTANT_BUFFER"},
        {D3D12_RESOURCE_STATE_INDEX_BUFFER, "INDEX_BUFFER"},
        {D3D12_RESOURCE_STATE_RENDER_TARGET, "RENDER_TARGET"},
        {D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, "PIXEL_SHADER_RESOURCE"},
	};
}

void DualDisplay::StaticInitialize(DXDevice* device) {
    device_ = device;
}

DualDisplay::DualDisplay(std::string debugName) {
	logger = getLogger("DualDisplay");
	debugName_ = debugName;
	logger->info("DualDisplay Created: {}", debugName_);
}

DualDisplay::~DualDisplay() {
    for (int i = 0; i < 2; ++i) {
        Displays_[i].textureData_->Release();
	}
}

void DualDisplay::Initialize(TextureData* data, TextureData* data2) {
    ID3D12Device* device = device_->GetDevice();
    DSVManager* dsvManager = device_->GetDSVManager();
    RTVManager* rtvManager = device_->GetRTVManager();

    if (data->GetSize() != data2->GetSize()) {
		throw "DualDisplay::Initialize TextureData size mismatch";
		return;
    }

    if(data->GetClearColor() != data2->GetClearColor()) {
        throw "DualDisplay::Initialize TextureData clearColor mismatch";
        return;
	}

    clearColor_ = data->GetClearColor();

    Displays_[0].textureData_ = data;
	Displays_[1].textureData_ = data2;

    for (int i = 0; i < 2; ++i) {
        width_ = data->GetSize().first;
        height_ = data->GetSize().second;

        //バリアの初期状態を設定
        Displays_[i].resourceState_ = D3D12_RESOURCE_STATE_PRESENT;

        //RTVの設定
        Displays_[i].rtvHandle_.UpdateHandle(rtvManager);

		rtvFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM;
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = rtvFormat_;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//2Dテクスチャとしてよみこむ
        device->CreateRenderTargetView(Displays_[i].textureData_->GetResource(), &rtvDesc, Displays_[i].rtvHandle_.GetCPU());

        //DSVの設定
        Displays_[i].dsvHandle_.UpdateHandle(dsvManager);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        
        Displays_[i].depthStencilResource_.Attach(CreateDepthStencilTextureResource(device, width_, height_));

        device->CreateDepthStencilView(Displays_[i].depthStencilResource_.Get(), &dsvDesc, Displays_[i].dsvHandle_.GetCPU());
    }
}

void DualDisplay::PreDraw(CommandObject* cmdObject, bool isClear) {
	auto commandList = cmdObject->GetCommandList();
    index_ = cmdObject->GetCommandIndex();

    EditBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    auto dsvCpu = Displays_[index_].dsvHandle_.GetCPU();
    auto rtvCpu = Displays_[index_].rtvHandle_.GetCPU();
    commandList->OMSetRenderTargets(1, &rtvCpu, false, &dsvCpu);

    if (isClear) {
        //レンダーターゲットのクリア
        commandList->ClearRenderTargetView(Displays_[index_].rtvHandle_.GetCPU(), &clearColor_.x, 0, nullptr);
        //デプスステンシルビューのクリア
        commandList->ClearDepthStencilView(Displays_[index_].dsvHandle_.GetCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        //ビューポート
        D3D12_VIEWPORT viewport{};
        //クライアント領域のサイズと一緒にして画面全体に表示
        viewport.Width = static_cast<float>(width_);
        viewport.Height = static_cast<float>(height_);
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        //シザー矩形
        D3D12_RECT scissorRect{};
        //基本的にビューポートと同じく刑が構成されるようにする
        scissorRect.left = 0;
        scissorRect.right = width_;
        scissorRect.top = 0;
        scissorRect.bottom = height_;

        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &scissorRect);
    }
}

void DualDisplay::ToTexture(CommandObject* cmdObject) {
	EditBarrier(cmdObject->GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DualDisplay::PostDraw(CommandObject* cmdObject) {
    EditBarrier(cmdObject->GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DualDisplay::EditBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES afterState) {
    logger->debug("{}::EditBarrier from {} to {}",
        debugName_,
        resourceStateToString[Displays_[index_].resourceState_],
        resourceStateToString[afterState]);
    InsertBarrier(commandList, afterState, Displays_[index_].resourceState_, Displays_[index_].textureData_->GetResource());
}