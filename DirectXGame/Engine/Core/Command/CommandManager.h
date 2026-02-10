#pragma once
#include <Core/DXDevice.h>
#include <Tool/Logger/Logger.h>
#include <Core/Command/Data/CommandObject.h>

namespace SHEngine::Command {

	class Manager {
	public:

		void Initialize(DXDevice* device);
		
		/// @brief コマンドオブジェクトを生成する
		/// @param type コマンドキューのタイプ
		/// @param index コマンドキューのインデックス
		std::unique_ptr<Object> CreateCommandObject(Type type, int index = 0);

		/// @brief コマンドを実行する
		/// @param type コマンドキューのタイプ
		/// @param index コマンドキューのインデックス
		void Execute(Type type, int index = 0);

		/// @brief シグナルを送る
		/// @param type コマンドキューのタイプ
		/// @param index コマンドキューのインデックス
		void SendSignal(Type type, int index = 0);

		/// @brief コマンドオブジェクトを解放するとき、Managerからも削除する
		void ReleaseObject(Type type, int index, int id);

	private:

		DXDevice* device_;
		Logger logger_;

		std::map<Type, std::vector<Microsoft::WRL::ComPtr<ID3D12CommandQueue>>> queue_{};

		// Type->QueueのIndex->CommandObjectまとめ
		std::map<Type, std::vector<std::map<int, Object*>>> commandObjects_;
		std::map<Type, int> nextIDsForObject_;
	};
}
