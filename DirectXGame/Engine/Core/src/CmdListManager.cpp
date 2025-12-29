#include "../CmdListManager.h"

void CmdListManager::Initialize(DXDevice* device) {
	device_ = device;
	srvManager_ = device->GetSRVManager();
	logger_ = getLogger("Engine");

	//コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr) && "Failed to create CommandQueue");


	auto commandObject = std::make_unique<CommandObject>();
	int id = commandObject->Initialize(device_->GetDevice(), this, srvManager_);
	commandObjects_[0] = std::move(commandObject);
}

void CmdListManager::Execute() {
	std::vector<ID3D12CommandList*> commandLists;
	auto beginObject = commandObjects_.begin();

	for (int i = 0; i < int(commandObjects_.size()); i) {
		commandLists.clear();

		for (int limit = i + 8; i < int(commandObjects_.size()) && i < limit; ++i) {
			auto commandList = beginObject->second->GetCommandList();
			commandList->Close();
			commandLists.push_back(commandList);
			++beginObject;
		}

		commandQueue_->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
	}
}

void CmdListManager::Reset() {
	for (const auto& [id, commandObject] : commandObjects_) {
		commandObject->Reset();
	}
}

std::shared_ptr<CommandObject> CmdListManager::CreateCommandObject() {
	return commandObjects_[0];
}
