#pragma once
#include "Data/CommandObject.h"
#include <Core/DXDevice.h>
#include <Tool/Logger/Logger.h>
#include <map>

class CmdListManager {
public:

	CmdListManager() = default;
	~CmdListManager() = default;

	void Initialize(DXDevice* device);
	void Execute();
	void Reset();

	void DeleteCommandObject(int id);
	
	std::unique_ptr<CommandObject> CreateCommandObject();
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

private:

	DXDevice* device_ = nullptr;
	Logger logger_ = nullptr;

	std::map<int, CommandObject*> commandObjects_;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	int frameChecker_ = 0;

};
