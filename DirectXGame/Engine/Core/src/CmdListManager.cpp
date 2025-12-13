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

	//コマンドオブジェクトの生成
	for (int i = 0; i < static_cast<int>(CmdListType::Count); ++i) {
		commandObjects_.emplace_back(std::make_unique<CommandObject>());
		commandObjects_.back()->Initialize(device_->GetDevice());
	}
}

void CmdListManager::Execute() {
	std::vector<ID3D12CommandList*> commandLists;
	for (const auto& commandObject : commandObjects_) {
		auto cmdList = commandObject->GetCommandList();
		cmdList->Close();
		commandLists.push_back(cmdList);
	}

	commandQueue_->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
}

void CmdListManager::Reset() {
	if (frameChecker_ < 2) {
		++frameChecker_;
		return;
	}

	for (const auto& commandObject : commandObjects_) {
		commandObject->Reset();
	}
}

CommandObject* CmdListManager::GetCommandObject(CmdListType type) {
	return commandObjects_[int(type)].get();
}
