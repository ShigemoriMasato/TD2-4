#include "CommandObject.h"
#include <cassert>

CommandObject::~CommandObject() {
}

void CommandObject::Initialize(ID3D12Device* device) {
    //コマンドリスト作成
    for (int i = 0; i < 2; ++i) {
        //CommandAllocator
        HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdLists_[i].commandAllocator_));
        //コマンドアロケータの生成がうまくいかなかったので起動できない
        assert(SUCCEEDED(hr));

        //CommandList
        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdLists_[i].commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&cmdLists_[i].commandList_));
        //コマンドリストの生成がうまくいかなかったので起動できない
        assert(SUCCEEDED(hr));
    }
}

ID3D12GraphicsCommandList* CommandObject::GetCommandList() const {
	return cmdLists_[index_].commandList_.Get();
}

void CommandObject::Reset() {
    HRESULT hr = cmdLists_[index_].commandAllocator_->Reset();
    assert(SUCCEEDED(hr));
    hr = cmdLists_[index_].commandList_->Reset(cmdLists_[index_].commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

void CommandObject::Swap() {
	index_ = (index_ + 1) % int(cmdLists_.size());
}
