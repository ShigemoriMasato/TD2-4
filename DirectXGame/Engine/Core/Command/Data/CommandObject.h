#pragma once
#include "CommandSet.h"

namespace SHEngine::Command {

	class Manager;

	class Object {
	public:

		Object() = default;
		~Object();

		/// @brief 初期化
		void Initialize(DXDevice* device, Manager* manager, Type type, int index, int id, int listNum);

		/// @brief コマンドを積めるかどうか
		bool CanExecute();
		/// @brief コマンドを積めるようになるまで待機する
		void WaitForCanExecute();

		/// @brief コマンドリストをリセットして、コマンドを積める状態にする(実行できる状態でなかったら実行できるまで待つ)
		void ResetCommandList();

		/// @brief コマンドリストを取得
		ID3D12GraphicsCommandList* GetCommandList() { return commandLists_[dxListIndex_].GetCommandList(); }

		/// @brief CommandListを実行する
		void Execute(std::vector<ID3D12CommandList*>& cmdLists);

		/// @brief Signalを送って、次のコマンドリストへ移動
		void SendSignal(ID3D12CommandQueue* executedCmdQueue);

		/// @brief 現在の状態を簡単に文字列であらわす
		std::string Log() const;

	private:

		int dxListIndex_ = 0;

		std::vector<DXList> commandLists_;
		DXDevice* device_ = nullptr;

		//デストラクタで解放する用
		Manager* manager_ = nullptr;
		Type type_;
		int queueIndex_ = 0;
		int id_ = 0;

		//コマンドリストの状態管理
		enum class State {
			Close,
			Open,
			Execute
		} state_;
	};

}
