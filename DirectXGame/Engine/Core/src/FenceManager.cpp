#include "../FenceManager.h"
#include <cassert>

void FenceManager::Initialize(ID3D12CommandQueue* queue, ID3D12Device* device) {
	commandQueue_ = queue;

    //初期値0でフェンスを作る
    HRESULT hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));

    //FenceのSignalを待つためのイベントを作成する
    fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent_ != nullptr);
}

void FenceManager::SendSignal() {
	++fenceValue_;
	commandQueue_->Signal(fence_.Get(), fenceValue_);
}

bool FenceManager::SignalChecker(int offset) {
	UINT64 waitValue = fenceValue_ - offset;
    
	//OverFlow対策
    if(waitValue > fenceValue_) {
        return true;
	}

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < UINT(waitValue)) {
        return false;
    }

    return true;
}

void FenceManager::WaitForGPU() {
    if (fence_->GetCompletedValue() < fenceValue_) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        //イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}

void FenceManager::Finalize() {
    SignalChecker(0);
}
