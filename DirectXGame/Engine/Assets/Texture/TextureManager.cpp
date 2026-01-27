#include "TextureManager.h"
#include <Utility/Color.h>
#include <Utility/DirectUtilFuncs.h>
#include <DirectXTex/d3dx12.h>
#include <Utility/SearchFile.h>

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
	errorTextureHandle_ = LoadTexture("Assets/.EngineResource/Texture/error.png");

	logger_ = getLogger("Engine");
}

void TextureManager::Clear() {
	textureDataList_.clear();
	uploadResources_.clear();
	intermediateResources_.clear();
}

void TextureManager::LoadAllTextures() {
	auto files = SearchFilePathsAddChild("Assets/Texture/", ".png");
	
	for (const auto& filePath : files) {
		LoadTexture(filePath);
	}
}

int TextureManager::GetTexture(std::string filePath) const {
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

	auto it = loadedTexturePaths_.find(factFilePath);
	if (it != loadedTexturePaths_.end()) {
		return it->second;
	}
	return errorTextureHandle_;
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

	//ファイルが存在しなかったらエラーテクスチャを返す
	if (!std::filesystem::exists(factFilePath)) {
		logger_->error("Texture File is Not Found: {}", factFilePath);
		return errorTextureHandle_;
	}

	//すでに読み込んでいたらそのハンドルを返す
	auto it = loadedTexturePaths_.find(factFilePath);
	if (it != loadedTexturePaths_.end()) {
		return it->second;
	}

	uploadResources_.push_back(textureData->Create(factFilePath, device_->GetDevice(), srvManager_));
	int offset = textureData->GetOffset();
	textureData->textureManager_ = this;
	textureDataList_[offset] = std::move(textureData);
	CheckMaxCount(offset);
	loadedTexturePaths_[factFilePath] = offset;
	return offset;
}

int TextureManager::CreateWindowTexture(uint32_t width, uint32_t height, uint32_t clearColor) {
	auto textureData = std::make_unique<TextureData>();
	Vector4 clearColorVec = ConvertColor(clearColor);
	textureData->Create(width, height, clearColorVec, device_->GetDevice(), srvManager_);
	int offset = textureData->GetOffset();
	textureData->textureManager_ = this;
	textureDataList_[offset] = std::move(textureData);
	return offset;
}

int TextureManager::CreateSwapChainTexture(ID3D12Resource* resource) {
	auto textureData = std::make_unique<TextureData>();
	textureData->Create(resource, device_->GetDevice(), srvManager_);
	textureData->textureManager_ = this;
	int offset = textureData->GetOffset();
	textureDataList_[offset] = std::move(textureData);
	return offset;
}

int TextureManager::CreateBitmapTexture(uint32_t width, uint32_t height, std::vector<uint32_t> colorMap) {
	auto textureData = std::make_unique<TextureData>();
	auto data = textureData->Create(width, height, colorMap, device_->GetDevice(), srvManager_);
	mipUploadData_.push_back(data);
	textureData->textureManager_ = this;
	int offset = textureData->GetOffset();
	textureDataList_[offset] = std::move(textureData);
	return offset;
}

void TextureManager::DeleteTexture(int handle) {
	auto it = textureDataList_.find(handle);
	if (it != textureDataList_.end()) {
		textureDataList_.erase(it);
	}
}

void TextureManager::DeleteTexture(TextureData* textureData) {
	for (const auto& [handle, data] : textureDataList_) {
		if (data.get() == textureData) {
			textureDataList_.erase(handle);
			return;
		}
	}
}

TextureData* TextureManager::GetTextureData(int handle) {
	return textureDataList_[handle].get();
}

void TextureManager::UploadTextures(ID3D12GraphicsCommandList* cmdList) {
	intermediateResources_.clear();
	for (const auto& [resource, mipImage] : uploadResources_) {
		intermediateResources_.push_back(nullptr);
		intermediateResources_.back().Attach(UploadTextureData(resource, mipImage, device_->GetDevice(), cmdList));
	}
	uploadResources_.clear();

	mipUploadingData_.clear();
	for (const auto& mipData : mipUploadData_) {
		mipUploadingData_.push_back(mipData);

		D3D12_TEXTURE_COPY_LOCATION dst{};
		dst.pResource = mipData.textureResource;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION src{};
		src.pResource = mipData.intermediateResource.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		device_->GetDevice()->GetCopyableFootprints(
			&mipData.desc, 0, 1, 0,
			&src.PlacedFootprint,
			nullptr, nullptr, nullptr
		);

		cmdList->CopyTextureRegion(
			&dst, 0, 0, 0,
			&src, nullptr
		);

		D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_COPY_DEST;
		InsertBarrier(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ, beforeState, mipData.textureResource);
	}

	mipUploadData_.clear();
}

void TextureManager::ClearUploadedResources() {
	intermediateResources_.clear();
}

void TextureManager::CheckMaxCount(int offset) {
	if (offset >= maxTextureCount) {
		assert(false && "I can't read Texture more!");
	}
}
