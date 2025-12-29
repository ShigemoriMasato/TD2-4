#pragma once
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <Tool/Logger/Logger.h>

#include <wrl.h>

#include "View/SRVManager.h"
#include "View/RTVManager.h"
#include "View/DSVManager.h"

#include <PSO/PSOEditor.h>

class DXDevice {
public:

	DXDevice() = default;
	~DXDevice();

	void Initialize();

	ID3D12Device* GetDevice() { return device_.Get(); }
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory_.Get(); }

	SRVManager* GetSRVManager() { return srvManager_.get(); }
	RTVManager* GetRTVManager() { return rtvManager_.get(); }
	DSVManager* GetDSVManager() { return dsvManager_.get(); }

	void SetPSO(ID3D12GraphicsCommandList* commandList, const PSOConfig& config);

private:

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	std::unique_ptr<SRVManager> srvManager_;
	std::unique_ptr<RTVManager> rtvManager_;
	std::unique_ptr<DSVManager> dsvManager_;

	std::unique_ptr<PSOEditor> psoEditor_;

private:

	Logger logger_;

};
