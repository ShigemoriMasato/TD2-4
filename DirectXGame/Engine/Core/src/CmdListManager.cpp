#include "../CmdListManager.h"

void CmdListManager::Initialize(DXDevice* device) {
	device_ = device;
	logger_ = LogSystem::getLogger("Engine");

	//コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr) && "Failed to create CommandQueue");
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
	if (frameChecker_ < 2) {
		++frameChecker_;
		return;
	}

	for (const auto& [id, commandObject] : commandObjects_) {
		commandObject->Reset();
	}
}

void CmdListManager::DeleteCommandObject(int id) {
	auto it = commandObjects_.find(id);
	
	if (it != commandObjects_.end()) {
		commandObjects_.erase(it);
		return;
	}

	logger_->warn("CmdListManager::DeleteCommandObject: Invalid CommandObject ID {}", id);
	assert(false && "CmdListManager::DeleteCommandObject: Invalid CommandObject ID");
}

std::unique_ptr<CommandObject> CmdListManager::CreateCommandObject() {
	auto commandObject = std::make_unique<CommandObject>();
	int id = commandObject->Initialize(device_->GetDevice(), this);
	commandObjects_[id] = commandObject.get();
	return std::move(commandObject);
}
