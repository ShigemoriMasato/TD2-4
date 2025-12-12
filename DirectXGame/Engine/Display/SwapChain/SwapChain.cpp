#include "SwapChain.h"

using namespace LogSystem;

void SwapChain::Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, WindowsApp* window) {
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

	ID3D12Resource* swapChainResources[2] = { nullptr, nullptr };
    //SwapChainからResourceを取得する
    hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

	int offset1 = textureManager->CreateSwapChainTexture(swapChainResources[0]);
	int offset2 = textureManager->CreateSwapChainTexture(swapChainResources[1]);

    Display display1;
	display1.Initialize(textureManager->GetTextureData(offset1), 0x00000000);
	Display display2;
	display2.Initialize(textureManager->GetTextureData(offset2), 0x00000000);

	logger_->info("=== Complete create SwapChain ===");
}
