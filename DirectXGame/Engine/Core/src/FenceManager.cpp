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

void FenceManager::WaitForSignal(int offset) {
	//FenceValueがマイナスになる場合、無限ループになる可能性があるため回避(UINTとintの比較は危険なため、256に範囲を絞って検証する)
	if (fenceValue_ < 256 && offset <= -256) {
		assert(false && "FenceManager::WaitForSignal offset is too small.");
		return;
	}

    int64_t waitFence = int64_t(fenceValue_);

    if (waitFence < 0) {
        return;
    }

	UINT64 waitValue = fenceValue_ - offset;

    //Fenceの値が指定したSignal値にたどり着いてるかを確認する
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < UINT(waitFence)) {
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(waitFence, fenceEvent_);
        //イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }

}

void FenceManager::Finalize() {
    WaitForSignal(0);
}
