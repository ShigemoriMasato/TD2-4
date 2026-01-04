#include "TextureManager.h"
#include <Utility/Color.h>
#include <Utility/DirectUtilFuncs.h>
#include <DirectXTex/d3dx12.h>

namespace {
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
	LoadTexture("Assets/.EngineResource/Texture/white1x1.png");
	LoadTexture("Assets/.EngineResource/Texture/uvChecker.png");
	LoadTexture("Assets/.EngineResource/Texture/error.png");

	logger_ = getLogger("TextureManager");
}

void TextureManager::Clear() {
	textureDatas_.clear();
	uploadResources_.clear();
	intermediateResources_.clear();
}

int TextureManager::LoadTexture(const std::string& filePath) {
	auto textureData = std::make_unique<TextureData>();

	std::string formatFirst = "Assets/";
	std::string factFilePath = "";
	if (filePath.length() < formatFirst.length()) {
		factFilePath = "Assets/Texture/" + filePath;
	} else {
		for (int i = 0; i < formatFirst.length(); ++i) {
			if (filePath[i] != formatFirst[i]) {
				factFilePath = "Assets/Texture/" + filePath;
				break;
			}

			if (i == formatFirst.length() - 1) {
				factFilePath = filePath;
			}
		}
	}

	if (!std::filesystem::exists(factFilePath)) {
		logger_->error("Texture File is Not Found: {}", factFilePath);
		return 2;//ErrorTexture
	}

	uploadResources_.push_back(textureData->Create(factFilePath, device_->GetDevice(), srvManager_));
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
	return offset;
}

int TextureManager::CreateSwapChainTexture(ID3D12Resource* resource) {
	auto textureData = std::make_unique<TextureData>();
	textureData->Create(resource, device_->GetDevice(), srvManager_);
	int offset = textureData->GetOffset();
	textureDatas_[offset] = std::move(textureData);
	return offset;
}

TextureData* TextureManager::GetTextureData(int handle) {
	return textureDatas_[handle].get();
}

void TextureManager::UploadTextures(ID3D12GraphicsCommandList* cmdList) {
	intermediateResources_.clear();
	for (const auto& [resource, mipImage] : uploadResources_) {
		intermediateResources_.push_back(nullptr);
		intermediateResources_.back().Attach(UploadTextureData(resource, mipImage, device_->GetDevice(), cmdList));
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
