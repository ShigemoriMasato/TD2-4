#pragma once
#include <wrl.h>
#include <d3d12.h>

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker();
};

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

void InsertBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES stateAfter, D3D12_RESOURCE_STATES& stateBefore, ID3D12Resource* pResource,
    D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
