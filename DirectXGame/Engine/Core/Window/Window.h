#pragma once
#include <Engine/Core/Device/DXDevice.h>

class Window {
public:



private:

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//スワップチェーンの設定
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr, nullptr };
	D3D12_RESOURCE_STATES resourcestates_[2] = { D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_PRESENT };
	float clearColor_[4] = { 0.0f,0.0f,0.0f,1.0f };

	bool isFrameFirst_ = true;	//PreDrawが初回かどうか
	bool initializeFrame_ = true; //セッション単位での初回かどうか

	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	uint64_t fenceValue;

	//Depth
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;


};
