#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Engine/Logger/Logger.h>

#include <wrl.h>
using namespace Microsoft::WRL;

class DXDevice {
public:

	DXDevice() = default;
	~DXDevice();

	void Initialize();

private:

	ComPtr<ID3D12Debug1> debugController_ = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	ComPtr<ID3D12Device> device_ = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

private:

	std::shared_ptr<spdlog::logger> logger_;

};
