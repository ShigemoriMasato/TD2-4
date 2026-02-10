#include "CommandObject.h"
#include <Core/Command/CommandManager.h>

using namespace SHEngine::Command;

Object::~Object() {
	manager_->ReleaseObject(type_, queueIndex_, id_);
}

void Object::Initialize(DXDevice* device, Manager* manager, Type type, int index, int id) {
	device_ = device;

	// コマンドリストを3つ作成
	constexpr int commandListCount = 3;
	commandLists_.resize(commandListCount);
	for (auto& cmdList : commandLists_) {
		cmdList.Initialize(device_);
	}

	// コマンドオブジェクトのタイプとキューインデックスを保存
	manager_ = manager;
	type_ = type;
	queueIndex_ = index;
	id_ = id;
}

bool Object::CanExecute() {
	// 現在のコマンドリストが実行可能かどうかを確認
	return commandLists_[dxListIndex_].CanExecute();
}

void Object::WaitForCanExecute() {
	// 現在のコマンドリストが実行可能になるまで待機
	commandLists_[dxListIndex_].WaitForCanExecute();
}

void SHEngine::Command::Object::Execute(std::vector<ID3D12CommandList*>& cmdLists) {
	// 現在のコマンドリストを取得
	auto& currentDXList = commandLists_[dxListIndex_];

	//実行可能か確かめる
	if (!currentDXList.CanExecute()) {
		return;
	}

	// コマンドリストを取得して閉じる
	ID3D12GraphicsCommandList* commandList = currentDXList.GetCommandList();
	HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr) && "Failed to close Command List");
	// コマンドリストを引数に追加
	cmdLists.push_back(commandList);
}

void SHEngine::Command::Object::SendSignal(ID3D12CommandQueue* executedCmdQueue) {
	// 現在のコマンドリストでシグナルを送信
	auto& currentDXList = commandLists_[dxListIndex_];
	currentDXList.SendSignal(executedCmdQueue);

	// 次のコマンドリストに移動（3つのコマンドリストを循環）
	dxListIndex_ = (dxListIndex_ + 1) % commandLists_.size();
}
