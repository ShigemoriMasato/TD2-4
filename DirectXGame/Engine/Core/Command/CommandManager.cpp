#include "CommandManager.h"

using namespace SHEngine::Command;

void Manager::Initialize(DXDevice* device) {
	device_ = device;
	logger_ = getLogger("Engine");

	//=========================================
	//コマンドキューの生成
	//=========================================
	// DirectQueue
	D3D12_COMMAND_QUEUE_DESC directQueueDesc{};
	auto& directQueue = queue_[Type::Direct].emplace_back();
	directQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	directQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	directQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(&directQueueDesc, IID_PPV_ARGS(&directQueue));
	assert(SUCCEEDED(hr) && "Failed to create CommandQueue");

	//CopyQueue
	auto& copyQueue = queue_[Type::Copy].emplace_back();
	D3D12_COMMAND_QUEUE_DESC copyQueueDesc{};
	copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = device_->GetDevice()->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&copyQueue));
	assert(SUCCEEDED(hr) && "Failed to create CommandQueue");

	//ComputeQueue(複数作成)
	D3D12_COMMAND_QUEUE_DESC computeQueueDesc{};
	computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	computeQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	for (int i = 0; i < 6; ++i) {
		auto& computeQueue = queue_[Type::Compute].emplace_back();
		hr = device_->GetDevice()->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeQueue));
	}

	//==========================================
	//コマンドオブジェクト管理用のコンテナ初期化
	//==========================================
	for (auto type : { Type::Direct, Type::Copy, Type::Compute }) {
		commandObjects_[type] = std::vector<std::map<int, Object*>>(queue_[type].size());
		nextIDsForObject_[type] = 0;
	}
}

std::unique_ptr<Object> SHEngine::Command::Manager::CreateCommandObject(Type type, int index) {
	std::unique_ptr<Object> commandObject = std::make_unique<Object>();
	int id = nextIDsForObject_[type]++;
	commandObject->Initialize(device_, this, type, index, id);

	// コマンドオブジェクトを管理用コンテナに登録
	if (type != Type::Compute) {
		index = 0; // DirectとCopyはキューが1つしかないのでindexを0に固定
	}
	commandObjects_[type][index][id] = commandObject.get();

	return std::move(commandObject);
}

void SHEngine::Command::Manager::Execute(Type type, int index) {
	std::vector<ID3D12CommandList*> cmdLists;
	// コマンドオブジェクトにコマンドリストを追加させる
	for (auto& [id, commandObject] : commandObjects_[type][index]) {
		commandObject->Execute(cmdLists);
	}

	// コマンドキューでコマンドリストを実行
	if (cmdLists.empty()) {
		return;
	}

	if (type != Type::Compute) {
		index = 0; // DirectとCopyはキューが1つしかないのでindexを0に固定
	}

	auto& cmdQueue = queue_[type][index];
	cmdQueue->ExecuteCommandLists(static_cast<UINT>(cmdLists.size()), cmdLists.data());
}

void SHEngine::Command::Manager::SendSignal(Type type, int index) {
	auto& cmdQueue = queue_[type][index];

	if (type != Type::Compute) {
		index = 0; // DirectとCopyはキューが1つしかないのでindexを0に固定
	}

	// 各コマンドオブジェクトにシグナルを送信させる
	for (auto& [id, commandObject] : commandObjects_[type][index]) {
		commandObject->SendSignal(cmdQueue.Get());
	}
}

void SHEngine::Command::Manager::ReleaseObject(Type type, int index, int id) {
	auto& objMap = commandObjects_[type][index];
	auto it = objMap.find(id);
	if (it != objMap.end()) {
		// コマンドオブジェクトを削除
		delete it->second;
		objMap.erase(it);
	}
}
