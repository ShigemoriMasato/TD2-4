#pragma once
#include "Data/CommandObject.h"
#include <Core/DXDevice.h>
#include <Tool/Logger/Logger.h>

//処理順
enum class CmdListType {
	Texture,

	SwapChain,

	Count
};

class CmdListManager {
public:

	CmdListManager() = default;
	~CmdListManager() = default;

	void Initialize(DXDevice* device);
	void Execute();
	void Reset();
	
	CommandObject* GetCommandObject(CmdListType type);

private:

	DXDevice* device_ = nullptr;
	Logger logger_ = nullptr;

	std::vector<std::unique_ptr<CommandObject>> commandObjects_;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	int frameChecker_ = 0;

};
