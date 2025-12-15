#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

class CmdListManager;

class CommandObject {
public:

	CommandObject() = default;
	~CommandObject();

	int Initialize(ID3D12Device* device, CmdListManager* manager);

	ID3D12GraphicsCommandList* GetCommandList() const;

	void Reset();

	//使用するコマンドリストを切り替える
	void Swap();

private:

	struct Command {
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	};
	std::vector<Command> cmdLists_;

	//次使うコマンドリストのインデックス
	int index_ = 0;

	static inline int nextCmdObjId_ = 0;
	int cmdObjId_ = nextCmdObjId_++;

	CmdListManager* manager_ = nullptr;
};
