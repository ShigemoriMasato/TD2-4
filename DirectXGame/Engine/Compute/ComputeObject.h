#pragma once
#include <Tool/Logger/Logger.h>
#include <Core/Command/CommandManager.h>
#include <Compute/PSO/CSPSOManager.h>

namespace SHEngine {

	class ComputeObject {
	public:

		ComputeObject(std::string debugName = "");
		~ComputeObject() = default;

		static void StaticInitialize(DXDevice* device, PSO::CSPSOManager* psoManager) { device_ = device, psoManager_ = psoManager; };
		void Initialize();

		void SetShader(std::string computeShaderName) { computeShaderName_ = computeShaderName; }
		void SetThreadGroupSize(int x, int y = 1, int z = 1) { threadGroupSize_ = { x, y, z }; }
		int CreateCBV(size_t bufferSize);
		int CreateSRV(size_t bufferSize, int num);
		int CreateUAV(size_t bufferSize, int num);

		void CopyBufferData(int index, const void* data, size_t dataSize);

		void GetUAVBuffer(int uavIndex, void* data, size_t dataSize);

		void Execute(CmdObj* cmdObj);

	private:

		static DXDevice* device_;
		static PSO::CSPSOManager* psoManager_;

		struct BufferData {
			void* mapped = nullptr;
			size_t size = 0;
		};

		std::vector<BufferData> mappedBuffers_{};

		std::vector<D3D12_GPU_VIRTUAL_ADDRESS> cbvAddress_{};
		std::vector<std::unique_ptr<SRVHandle>> srvHandle_{};
		std::vector<std::unique_ptr<SRVHandle>> uavHandle_{};
		std::string computeShaderName_;

		struct ThreadGroupSize {
			int x;
			int y;
			int z;
		} threadGroupSize_;

		struct Resource {
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		};
		std::vector<Resource> resources_{};

		// UAV用のReadbackバッファ
		struct UAVReadback {
			Microsoft::WRL::ComPtr<ID3D12Resource> uavResource;
			Microsoft::WRL::ComPtr<ID3D12Resource> readbackResource;
			size_t bufferSize = 0;
		};
		std::vector<UAVReadback> uavReadbacks_{};

		//実行が終わったか確認する用。絶対に実行には使用しない
		CmdObj* cmdObj_ = nullptr;

		Logger logger_ = getLogger("Compute");
		std::string debugName_;
	};

}
