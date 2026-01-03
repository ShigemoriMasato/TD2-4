#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class FenceManager {
public:

	FenceManager() = default;
	~FenceManager() = default;

	void Initialize(ID3D12CommandQueue* queue, ID3D12Device* device);

	void SendSignal();

	bool SignalChecker(int offset = 0);
	//GPUの処理が完全に完了するまで待機する
	void WaitForGPU();

	void Finalize();

private:

	ID3D12CommandQueue* commandQueue_ = nullptr;

	UINT64 fenceValue_ = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	HANDLE fenceEvent_ = nullptr;
};
