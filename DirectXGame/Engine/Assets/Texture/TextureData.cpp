#include "TextureData.h"
#include <DirectXTex/d3dx12.h>
#include <Utility/ConvertString.h>
#include <Assets/Texture/TextureManager.h>

using namespace Microsoft::WRL;

namespace {
	DirectX::ScratchImage CreateMipImages(const std::string& filePath) {
		//テクスチャファイルを読んでプログラムで扱えるようにする
		DirectX::ScratchImage image{};
		std::wstring filePathW = ConvertString(filePath);
		HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr) && "Failed to Open TextureFile");

		DirectX::ScratchImage mipImages{};
		//ミニマップの作成(画像サイズが最小の場合、作成手順を飛ばす)
		if (image.GetMetadata().mipLevels > 1) {
			hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
			assert(SUCCEEDED(hr));
		} else {
			mipImages = std::move(image);
		}
		return mipImages;
	}

	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
		//metadataを基にResourceの設定
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Width = UINT(metadata.width);
		resourceDesc.Height = UINT(metadata.height);
		resourceDesc.MipLevels = UINT16(metadata.mipLevels);
		resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
		resourceDesc.Format = metadata.format;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

		//利用するHeapの設定。非常に特殊な運用。
		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//Resourceの生成
		ID3D12Resource* resource = nullptr;
		HRESULT hr = device->CreateCommittedResource(
			&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
			IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));
		return resource;
	}
}

void TextureData::Release() {
	textureManager_->DeleteTexture(this);
}

void TextureData::Create(uint32_t width, uint32_t height, Vector4 clearColor, ID3D12Device* device, SRVManager* srvManager) {
	//PostEffect用のリソースの作成
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;              // defaultのヒープを使用

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	for (int i = 0; i < 4; ++i) {
		clearValue.Color[i] = clearColor[i];
	}

	HRESULT hr = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PRESENT,
		&clearValue,
		IID_PPV_ARGS(&textureResource_)
	);
	assert(SUCCEEDED(hr) && "Failed to create off-screen resource");

	clearColor_ = clearColor;

	// metadataがないのでフォーマットとミップ数は手動設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//!mipmapを使うかどうかは今後要検討
	srvDesc.Texture2D.MipLevels = 1;

	// SRV用ディスクリプタ位置を確保
	srvHandle_.UpdateHandle(srvManager, 0);

	// SRVを作成
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, srvHandle_.GetCPU());

	width_ = width;
	height_ = height;

	textureResource_->SetName(LPCWSTR(ConvertString("WindowTexture : " + std::to_string(debugTextureCount++)).c_str()));
}

void TextureData::Create(ID3D12Resource* resource, ID3D12Device* device, SRVManager* manager) {
	textureResource_.Attach(resource);

	// metadataがないのでフォーマットとミップ数は手動設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//!mipmapを使うかどうかは今後要検討
	srvDesc.Texture2D.MipLevels = 1;

	// SRV用ディスクリプタ位置を確保
	srvHandle_.UpdateHandle(manager, 0);

	// SRVを作成
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, srvHandle_.GetCPU());

	auto desc = resource->GetDesc();

	width_ = static_cast<uint32_t>(desc.Width);
	height_ = static_cast<uint32_t>(desc.Height);

	textureResource_->SetName(LPCWSTR(ConvertString("WindowTexture : " + std::to_string(debugTextureCount++)).c_str()));
}

TextureData::MipMapUploadData TextureData::Create(uint32_t width, uint32_t height, std::vector<uint32_t> colorMap, ID3D12Device* device, SRVManager* srvManager) {
	MipMapUploadData mipData{};

	//TextureResource
	D3D12_RESOURCE_DESC& desc = mipData.desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);

	device->CreateCommittedResource(
		&heap,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&textureResource_)
	);

	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvHandle_.UpdateHandle(srvManager, 0);
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, srvHandle_.GetCPU());

	//アップロード用バッファの作成
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource_.Get(), 0, 1);

	uint64_t uploadSize = 0;
	device->GetCopyableFootprints(
		&desc, 0, 1, 0,
		nullptr, nullptr, nullptr,
		&uploadSize
	);

	D3D12_HEAP_PROPERTIES uploadHeap{};
	uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC uploadDesc{};
	uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.Width = uploadSize;
	uploadDesc.Height = 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels = 1;
	uploadDesc.SampleDesc.Count = 1;
	uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	device->CreateCommittedResource(
		&uploadHeap,
		D3D12_HEAP_FLAG_NONE,
		&uploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mipData.intermediateResource)
	);

	//テクスチャデータを書き込む
	void* mapped = nullptr;
	mipData.intermediateResource->Map(0, nullptr, &mapped);
	std::memcpy(mapped, colorMap.data(), colorMap.size() * sizeof(uint32_t));
	mipData.intermediateResource->Unmap(0, nullptr);

	mipData.textureResource = textureResource_.Get();

	return mipData;
}

std::pair<ID3D12Resource*, DirectX::ScratchImage> TextureData::Create(std::string filePath, ID3D12Device* device, SRVManager* srvManager) {
	//TextureResourceを作成
	DirectX::ScratchImage mipImages = CreateMipImages(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	textureResource_.Attach(CreateTextureResource(device, metadata));

	//画像サイズの取得
	width_ = static_cast<uint32_t>(metadata.width);
	height_ = static_cast<uint32_t>(metadata.height);

	//metadataをもとにSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	srvHandle_.UpdateHandle(srvManager, 0);

	//SRVを作成する
	device->CreateShaderResourceView(textureResource_.Get(), &srvDesc, srvHandle_.GetCPU());

	std::pair<ID3D12Resource*, DirectX::ScratchImage> result = { textureResource_.Get(), std::move(mipImages) };
	return result;
}
