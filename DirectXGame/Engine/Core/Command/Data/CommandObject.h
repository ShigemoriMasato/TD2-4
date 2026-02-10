#pragma once
#include "CommandSet.h"

namespace SHEngine::Command {

	enum class Type {
		Direct,
		Copy,
		Compute
	};

	class Manager;

	class Object {
	public:

		/// @brief 初期化
		void Initialize(DXDevice* device, Manager* manager, Type type, int index, int id);

		/// @brief コマンドを積めるかどうか
		bool CanExecute();
		/// @brief コマンドを積めるようになるまで待機する
		void WaitForCanExecute();

		/// @brief コマンドリストを取得
		ID3D12GraphicsCommandList* GetCommandList() { return commandLists_[dxListIndex_].GetCommandList(); }

		/// @brief CommandListを実行する
		void Execute(std::vector<ID3D12CommandList*>& cmdLists);

		/// @brief Signalを送って、次のコマンドリストへ移動
		void SendSignal(ID3D12CommandQueue* executedCmdQueue);

	private:

		int dxListIndex_ = 0;

		std::vector<DXList> commandLists_;
		DXDevice* device_ = nullptr;

		//デストラクタで解放する用
		Manager* manager_ = nullptr;
		Type type_;
		int queueIndex_ = 0;
		int id_ = 0;
	};

}
