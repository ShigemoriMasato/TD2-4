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

		// @brief Shader名を登録
		void SetShader(std::string computeShaderName) { computeShaderName_ = computeShaderName; }
		// @brief スレッドグループのサイズを登録
		void SetThreadGroupSize(int x, int y = 1, int z = 1) { threadGroupSize_ = { x, y, z }; }
		// @brief CBVを作成して登録。
		// @param bufferSize バッファのサイズ
		// @return 登録したCBVのインデックス。コピーする際に使用
		int CreateCBV(size_t bufferSize);
		// @brief SRVを作成して登録。
		// @param bufferSize バッファのサイズ
		// @param num バッファの数
		// @return 登録したSRVのインデックス。コピーする際に使用
		int CreateSRV(size_t bufferSize, int num);

		// @brief UAVを作成して登録。コピーインデックスではなく、UAVインデックスを返す。UAVインデックスはSRVやCBVと違い、UAVの数だけ存在する。
		// @param bufferSize バッファのサイズ
		// @param num バッファの数
		// @return 登録したUAVのインデックス。値を取得する際に使用。
		int CreateUAV(size_t bufferSize, int num);

		// @brief 登録したCBV/SRV/UAVにデータをコピーする。UAVの場合はCopyBufferDataではなくCopyBufferDataToUAVを使用すること。
		// @param index 登録したCBV/SRV/UAVのインデックス
		// @param data コピーするデータのポインタ
		// @param dataSize コピーするデータのサイズ
		void CopyBufferData(int index, const void* data, size_t dataSize);

		// @brief 登録したUAVからデータを取得する。CopyBufferDataではなくGetUAVBufferを使用すること。
		// @param uavIndex 登録したUAVのインデックス
		// @param data データを取得するためのバッファのポインタ
		// @param dataSize 取得するデータのサイズ
		void GetUAVBuffer(int uavIndex, void* data, size_t dataSize);

		// @brief 登録したCBV/SRV/UAVをComputeShaderにセットして、ComputeShaderを実行する。
		// @param cmdObj コマンドオブジェクト。この関数の後にengine_->Execute、cmdObj->WaitForCanExecuteを呼び出すこと。
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
