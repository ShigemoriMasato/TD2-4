#include "../SwapChain.h"

using namespace LogSystem;

void SwapChain::Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, WindowsApp* window, uint32_t clearColor) {
    logger_ = getLogger("Engine");

	logger_->info("=== Start create SwapChain ===");

	HWND hwnd = window->GetHwnd();
	auto [width, height] = window->GetWindowSize();
    
    //スワップチェーンを生成する
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = width;	                        //ウィンドウ幅
    swapChainDesc.Height = height;	                        //ウィンドウ高さ
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      	    //色の形式
    swapChainDesc.SampleDesc.Count = 1;	                            //マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2;	//ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   	//モニタに映したら捨てる

    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT hr = device->GetDxgiFactory()->CreateSwapChainForHwnd(
        commandQueue,		        		                                //コマンドキュー
        hwnd,			                                                    //ウィンドウハンドル
        &swapChainDesc,	        		                                    //設定
        nullptr,		    	    		                                //モニタの設定
        nullptr,			    		                                    //出力の設定
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));	//スワップチェーンのポインタ
    assert(SUCCEEDED(hr));

    logger_->info("Complete create SwapChain");

	ID3D12Resource* swapChainResources;
    for(int i = 0; i < 2; i++) {
        //SwapChainからResourceを取得する
        hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources));
        assert(SUCCEEDED(hr));

        //TextureManagerにSwapChain用Textureを作成してもらう
        int handle = textureManager->CreateSwapChainTexture(swapChainResources);
		TextureData* data = textureManager->GetTextureData(handle);

        //Displayを初期化する
		displays_[i] = std::make_unique<Display>();
        displays_[i]->Initialize(data, clearColor);
	}

    logger_->debug("Initialized  id : {}    backBuffer : {}", debugID_, currentBackBufferIndex_);
	logger_->info("SwapChain Info: Width : {} Height : {} ClearColor : {:08X}", width, height, clearColor);

    logger_->info("=== Complete create SwapChain ===");
}

void SwapChain::PreDraw(ID3D12GraphicsCommandList* cmdList, bool isClear) {
    currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

	displays_[currentBackBufferIndex_]->PreDraw(cmdList, isClear);

    logger_->debug("PreDraw  id : {}    backBuffer : {}", debugID_, currentBackBufferIndex_);

}

void SwapChain::ToTexture(ID3D12GraphicsCommandList* cmdList) {
	displays_[currentBackBufferIndex_]->ToTexture(cmdList);
}

void SwapChain::PostDraw(ID3D12GraphicsCommandList* cmdList) {
    currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

    displays_[currentBackBufferIndex_]->PostDraw(cmdList);

    logger_->debug("PostDraw  id : {}    backBuffer : {}", debugID_, currentBackBufferIndex_);

}

void SwapChain::Present() {
    currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

    logger_->debug("Present  id : {} BackBuffer : {}", debugID_, currentBackBufferIndex_);
    //画面に表示する
    HRESULT hr = swapChain_->Present(1, 0);
	assert(SUCCEEDED(hr));

    currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}
