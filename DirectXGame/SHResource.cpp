#include "SHResource.h"
#include <Utility/DirectUtilFuncs.h>
#include <DirectXTex/d3dx12.h>
#define Sigma(x) for (int i = 0; i < x; ++i)

SHEngine::GPUBuffer::GPUBuffer(ResourceDesc& desc) {
	const size_t size = (desc.sizeInBytes + 255) & ~255;	//256の倍数に揃える

	for (int i = 0; i < desc.bufferNum; ++i) {

		//頂点リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = ((desc.bufferType & BufferType::UAV) == 0U ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
		//頂点リソースの設定
		D3D12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
		//バッファリソース、テクスチャの場合はまた別の設定をする
		bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferResourceDesc.Width = size;
		//バッファの場合はこれにする決まり
		bufferResourceDesc.Height = 1;
		bufferResourceDesc.DepthOrArraySize = 1;
		bufferResourceDesc.MipLevels = 1;
		bufferResourceDesc.SampleDesc.Count = 1;
		//バッファの場合はこれにする決まり
		bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Resource* bufferResource = nullptr;

		HRESULT reason = device_->GetDevice()->GetDeviceRemovedReason();

		HRESULT hr = device_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&bufferResource));
		assert(SUCCEEDED(hr));

	}

#ifndef BufferCheck
#define BufferCheck(type) if (bufferType & static_cast<uint8_t>(BufferType::type))
#endif

	uint8_t bufferType = desc.bufferType & 0b111;

	while (bufferType != 0) {
		BufferCheck(CBV) {
			Sigma(desc.bufferNum) {
				descriptorHandles_[BufferType::CBV].push_back(static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>(resources_[i].res->GetGPUVirtualAddress()));
			}
		} else BufferCheck(SRV) {
			Sigma(desc.bufferNum) {
				auto& res = resources_[i].res;

				//SRVハンドルの取得
				std::unique_ptr<SRVHandle>& srvHandle = srvHandles_.emplace_back();
				srvHandle = std::make_unique<SRVHandle>();
				srvHandle->UpdateHandle(device_->GetSRVManager());

				//ParticleDataのSRV作成
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				srvDesc.Buffer.NumElements = desc.elementCount;
				srvDesc.Buffer.StructureByteStride = desc.sizeInBytes;

				device_->GetDevice()->CreateShaderResourceView(res.Get(), &srvDesc, srvHandle->GetCPU());

				descriptorHandles_[BufferType::SRV].push_back(srvHandle->GetGPU());
			}
		} else BufferCheck(UAV) {
			Sigma(desc.bufferNum) {
				auto& res = resources_[i].res;

				//SRVハンドルの取得
				std::unique_ptr<SRVHandle>& uavHandle = uavHandles_.emplace_back();
				uavHandle = std::make_unique<SRVHandle>();
				uavHandle->UpdateHandle(device_->GetSRVManager());

				//ParticleDataのSRV作成
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.NumElements = desc.elementCount;
				uavDesc.Buffer.CounterOffsetInBytes = 0;
				uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
				uavDesc.Buffer.StructureByteStride = desc.sizeInBytes;

				device_->GetDevice()->CreateUnorderedAccessView(res.Get(), nullptr, &uavDesc, uavHandle->GetCPU());

				descriptorHandles_[BufferType::UAV].push_back(uavHandle->GetGPU());

			}
		} else {
			assert(false && "Invalid buffer type");
		}
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE SHEngine::GPUBuffer::GetGPUDescriptorHandle(BufferType type, uint32_t bufferIndex) const {
	return descriptorHandles_.at(type).at(bufferIndex);
}

void SHEngine::GPUBuffer::CopyBuffer(const void* data, size_t dataSize, uint32_t bufferIndex) {
	//安全性担保
	assert(bufferIndex < resources_.size());
	bufferIndex = bufferIndex % resources_.size();
	if(dataSize > sizeInBytes_) {
		assert(false && "Data size exceeds buffer size");
		return;
	}

	//データのコピー
	std::memcpy(mappedData_[bufferIndex], data, dataSize);
}

BufferType operator|(BufferType a, BufferType b) {
	return static_cast<BufferType>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

BufferType operator|(uint8_t a, BufferType b) {
	return static_cast<BufferType>(a | static_cast<uint8_t>(b));
}

uint8_t operator&(uint8_t a, BufferType b) {
	return (a & static_cast<uint8_t>(b));
}

uint8_t operator~(BufferType a) {
	return ~static_cast<uint8_t>(a);
}
