#pragma once
#include <Tool/Logger/Logger.h>
#include <Core/Command/CommandManager.h>

namespace SHEngine {

	class ComputeObject {
	public:

		ComputeObject(std::string debugName = "");
		~ComputeObject() = default;

		void StaticInitialize(DXDevice* device) { device_ = device; };
		void Initialize();

		int CreateCBV(size_t bufferSize);
		int CreateSRV(size_t bufferSize, int num);
		int CreateUAV(size_t bufferSize, int num);

		void CopyBufferData(int index, const void* data, size_t dataSize);

		void GetUAVBuffer(int index, void* data, size_t dataSize);

		void Execute(CmdObj* cmdObj);

	private:

		static DXDevice* device_;

		struct BufferData {
			void* mapped = nullptr;
			size_t size = 0;
		};

		std::vector<BufferData> mappedBuffers_{};

		std::vector<D3D12_GPU_VIRTUAL_ADDRESS> cbvAddress_{};
		std::vector<std::unique_ptr<SRVHandle>> srvHandle_{};
		std::vector<std::unique_ptr<SRVHandle>> uavHandle_{};

		struct Resource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		};
		std::vector<Resource> resources_{};

		int index_ = 0;

		//実行が終わったか確認する用。絶対に実行には使用しない
		CmdObj* cmdObj_ = nullptr;

		Logger logger_ = getLogger("Compute");
		std::string debugName_;
	};

}
