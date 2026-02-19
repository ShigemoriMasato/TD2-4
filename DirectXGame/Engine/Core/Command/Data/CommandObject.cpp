#include "CommandObject.h"
#include <Core/Command/CommandManager.h>

using namespace SHEngine::Command;

Object::~Object() {
	manager_->ReleaseObject(type_, queueIndex_, id_);
}

void Object::Initialize(DXDevice* device, Manager* manager, Type type, int index, int id, int listNum) {
	device_ = device;

	// コマンドリストを3つ作成
	commandLists_.resize(listNum);
	for (auto& cmdList : commandLists_) {
		cmdList.Initialize(device_, type);
	}

	// コマンドオブジェクトのタイプとキューインデックスを保存
	manager_ = manager;
	type_ = type;
	queueIndex_ = index;
	id_ = id;

	ResetCommandList();
}

bool Object::CanExecute() {
	// 現在のコマンドリストが実行可能かどうかを確認
	return commandLists_[dxListIndex_].CanExecute();
}

void Object::WaitForCanExecute() {
	// 現在のコマンドリストが実行可能になるまで待機
	commandLists_[dxListIndex_].WaitForCanExecute();
}

void SHEngine::Command::Object::ResetCommandList() {
	if(state_ == State::Open) {
		// コマンドリストが開いている場合はリセットするとエラーになるのでリセットしない
		return;
	}

	if(state_ == State::Execute) {
		// コマンドリストが実行された後は、シグナルを送ってからリセットする
		SendSignal(manager_->GetCommandQueue(type_, queueIndex_));
	}

	commandLists_[dxListIndex_].ResetCommandList();
	state_ = State::Open;
}

void SHEngine::Command::Object::Execute(std::vector<ID3D12CommandList*>& cmdLists) {
	if(state_ == State::Close) {
		//実行できるようにリセットする
		ResetCommandList();
	}

	// 現在のコマンドリストを取得
	auto& currentDXList = commandLists_[dxListIndex_];

	// コマンドリストを取得して閉じる
	ID3D12GraphicsCommandList* commandList = currentDXList.GetCommandList();
	HRESULT hr = commandList->Close();
	assert(SUCCEEDED(hr) && "Failed to close Command List");
	// コマンドリストを引数に追加
	cmdLists.push_back(commandList);

	state_ = State::Execute;
}

void SHEngine::Command::Object::SendSignal(ID3D12CommandQueue* executedCmdQueue) {
	// 現在のコマンドリストでシグナルを送信
	auto& currentDXList = commandLists_[dxListIndex_];
	currentDXList.SendSignal(executedCmdQueue);

	// 次のコマンドリストに移動（3つのコマンドリストを循環）
	dxListIndex_ = (dxListIndex_ + 1) % commandLists_.size();

	state_ = State::Close;
}

std::string SHEngine::Command::Object::Log() const {
	std::string ans;
	ans = "CommandObject - Type: " + std::to_string(static_cast<int>(type_)) +
		", QueueIndex: " + std::to_string(queueIndex_) +
		", ID: " + std::to_string(id_) +
		", CurrentDXListIndex: " + std::to_string(dxListIndex_);
	return ans;
}
