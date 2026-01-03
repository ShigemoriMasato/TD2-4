#pragma once
#include <Core/DXDevice.h>
#include <Utility/DirectUtilFuncs.h>

struct DrawData {
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vbv;
	D3D12_INDEX_BUFFER_VIEW ibv;
	uint32_t vertexNum;
	uint32_t indexNum;
};

class DrawDataManager {
public:

	void Initialize(DXDevice* device);
	
	template <typename T>
	void AddVertexBuffer(const std::vector<T>& data);
	void AddIndexBuffer(std::vector<uint32_t> indices);
	int CreateDrawData();

	DrawData GetDrawData(int index) const {
		assert(index >= 0 && index < static_cast<int>(drawDatas_.size()) && "DrawDataManager::GetDrawData: Invalid DrawData index");
		return drawDatas_[index];
	}

private:

	DXDevice* device_ = nullptr;
	Logger logger_ = nullptr;

	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	uint32_t vertexNum_ = 0;
	uint32_t indexNum_ = 0;
	std::vector<DrawData> drawDatas_;

	struct Resource {
		Microsoft::WRL::ComPtr<ID3D12Resource> res;
	};
	std::vector<Resource> resources_;

};

template<typename T>
inline void DrawDataManager::AddVertexBuffer(const std::vector<T>& data) {
	Resource res{};
	size_t bufferSize = ((sizeof(T) * data.size() + 255) & ~255);
	res.res.Attach(CreateBufferResource(device_->GetDevice(), bufferSize));	//256の倍数に揃えたうえでnum倍
	resources_.push_back(res);

	//データ転送
	void* mapped = nullptr;
	
	HRESULT hr = res.res->Map(0, nullptr, &mapped);
	assert(SUCCEEDED(hr));
	std::memcpy(mapped, data.data(), sizeof(T) * data.size());
	res.res->Unmap(0, nullptr);
	
	//VBV作成
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	vbv.BufferLocation = res.res->GetGPUVirtualAddress();
	vbv.SizeInBytes = static_cast<UINT>(bufferSize);
	vbv.StrideInBytes = sizeof(T);

	vertexBufferViews_.push_back(vbv);
	vertexNum_ = static_cast<int>(data.size());

	logger_->debug("Vertex Buffer Added: {} vertices", data.size());
}
