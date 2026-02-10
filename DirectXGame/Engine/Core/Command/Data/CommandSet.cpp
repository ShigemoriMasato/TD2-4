#include "CommandSet.h"

using namespace SHEngine::Command;

SHEngine::Command::DXList::~DXList() {
	// フェンスイベントのクローズ
	if (fenceEvent_) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
}

void DXList::Initialize(DXDevice* device) {
	device_ = device;
	// コマンドアロケータの作成
	HRESULT hr = device_->GetDevice()->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(hr) && "Failed to create Command Allocator");

	// コマンドリストの作成
	hr = device_->GetDevice()->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_)
	);

	assert(SUCCEEDED(hr) && "Failed to create Command List");
	// コマンドリストは初期状態で記録モードなので閉じておく
	hr = commandList_->Close();
	assert(SUCCEEDED(hr) && "Failed to close Command List");



	// フェンスの作成
	hr = device_->GetDevice()->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence_)
	);
	assert(SUCCEEDED(hr) && "Failed to create Fence");
	// フェンスイベントの作成
	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(fenceEvent_ && "Failed to create Fence Event");

	// 初期フェンス値
	fenceValue_ = 0;
}

bool DXList::CanExecute() {
	//シグナルを受け取る
	UINT64 gpuFenceValue = fence_->GetCompletedValue();
	//フェンス値と比較
	if (gpuFenceValue >= fenceValue_) {
		return true;
	}
	return false;
}

void DXList::WaitForCanExecute() {
	//シグナルを受け取る
	UINT64 gpuFenceValue = fence_->GetCompletedValue();
	//フェンス値と比較
	if (gpuFenceValue < fenceValue_) {
		//完了していなければイベントをセットして待機
		HRESULT hr = fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		assert(SUCCEEDED(hr) && "Failed to set event on completion");
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}
