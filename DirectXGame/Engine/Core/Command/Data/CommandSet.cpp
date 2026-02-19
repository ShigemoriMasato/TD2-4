#include "CommandSet.h"

using namespace SHEngine::Command;

SHEngine::Command::DXList::~DXList() {
	// フェンスイベントのクローズ
	if (fenceEvent_) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
}

void DXList::Initialize(DXDevice* device, Type type) {
	device_ = device;

	D3D12_COMMAND_LIST_TYPE commandListType{};

	switch (type) {
	case Type::Direct:
		commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case Type::Texture:
		commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case Type::Compute:
		commandListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}

	// コマンドアロケータの作成
	HRESULT hr = device_->GetDevice()->CreateCommandAllocator(
		commandListType,
		IID_PPV_ARGS(&commandAllocator_)
	);
	assert(SUCCEEDED(hr) && "Failed to create Command Allocator");

	// コマンドリストの作成
	hr = device_->GetDevice()->CreateCommandList(
		0,
		commandListType,
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

void DXList::SendSignal(ID3D12CommandQueue* commandQueue) {
	//コマンドキューにシグナルを送る
	HRESULT hr = commandQueue->Signal(fence_.Get(), ++fenceValue_);
	assert(SUCCEEDED(hr) && "Failed to signal Command Queue");
}

void DXList::ResetCommandList() {
	WaitForCanExecute(); // コマンドリストが実行可能になるまで待機

	//コマンドリストとアロケータをリセット
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr) && "Failed to reset Command Allocator");
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr) && "Failed to reset Command List");

	//Heapのセット
	auto srvHeap = device_->GetSRVManager()->GetHeap();
	commandList_->SetDescriptorHeaps(1, &srvHeap);
}
