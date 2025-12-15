#include "TextureManager.h"
#include <Utility/Color.h>
#include <DirectXTex/d3dx12.h>

namespace {

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
		//頂点リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//uploadHeapを使う
		//頂点リソースの設定
		D3D12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);
		//バッファリソース、テクスチャの場合はまた別の設定をする
		bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferResourceDesc.Width = sizeInBytes;
		//バッファの場合はこれにする決まり
		bufferResourceDesc.Height = 1;
		bufferResourceDesc.DepthOrArraySize = 1;
		bufferResourceDesc.MipLevels = 1;
		bufferResourceDesc.SampleDesc.Count = 1;
		//バッファの場合はこれにする決まり
		bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Resource* bufferResource = nullptr;

		HRESULT reason = device->GetDeviceRemovedReason();

		HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&bufferResource));
		assert(SUCCEEDED(hr));

		return bufferResource;
	}

	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		HRESULT hr = DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
		assert(SUCCEEDED(hr));
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
		ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
		UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrier);
		return intermediateResource;
	}
}

void TextureManager::Initialize(DXDevice* device, CmdListManager* CmdListManager) {
	device_ = device;
	srvManager_ = device->GetSRVManager();
	cmdObject_ = CmdListManager->CreateCommandObject();
}

void TextureManager::Clear() {
	textureDatas_.clear();
	uploadResources_.clear();
	intermediateResources_.clear();
}

int TextureManager::LoadTexture(const std::string& filePath) {
	auto textureData = std::make_unique<TextureData>();
	auto uploadData = textureData->Create(filePath, device_->GetDevice(), srvManager_);
	int offset = textureData->GetOffset();
	textureDatas_[offset] = std::move(textureData);
	CheckMaxCount(offset);
	return offset;
}

int TextureManager::CreateWindowTexture(uint32_t width, uint32_t height, uint32_t clearColor) {
	auto textureData = std::make_unique<TextureData>();
	Vector4 clearColorVec = ConvertColor(clearColor);
	textureData->Create(width, height, clearColorVec, device_->GetDevice(), srvManager_);
	int offset = textureData->GetOffset();
	textureDatas_[offset] = std::move(textureData);
	CheckMaxCount(offset);
	return offset;
}

int TextureManager::CreateSwapChainTexture(ID3D12Resource* resource) {
	auto textureData = std::make_unique<TextureData>();
	textureData->Create(resource, device_->GetDevice(), srvManager_);
	int offset = textureData->GetOffset();
	textureDatas_[offset] = std::move(textureData);
	CheckMaxCount(offset);
	return offset;
}

TextureData* TextureManager::GetTextureData(int handle) {
	return textureDatas_[handle].get();
}

void TextureManager::UploadTextures(ID3D12GraphicsCommandList* cmdList) {
	for(const auto& [resource, mipImage] : uploadResources_) {
		auto intermediateResource = UploadTextureData(resource, mipImage, device_->GetDevice(), cmdList);
		intermediateResources_.push_back(intermediateResource);
	}
	uploadResources_.clear();
}

void TextureManager::ClearUploadedResources() {
	intermediateResources_.clear();
}

void TextureManager::CheckMaxCount(int offset) {
	if (offset >= maxTextureCount) {
		assert(false && "I can't read Texture more!");
	}
}
