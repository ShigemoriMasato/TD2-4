#pragma once
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

struct Command {
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
};
