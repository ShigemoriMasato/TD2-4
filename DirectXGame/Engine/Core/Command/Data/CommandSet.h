#pragma once
#include <Core/DXDevice.h>

namespace SHEngine::Command {
	class DXList {
	public:

		~DXList();

		/// @brief 初期化
		void Initialize(DXDevice* device);

		/// @brief コマンドを積めるかどうか
		bool CanExecute();
		/// @brief コマンドを積めるようになるまで待機する
		void WaitForCanExecute();

		/// @brief コマンドリストを取得
		ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

		void SendSignal(ID3D12CommandQueue* commandQueue);

	private:

		DXDevice* device_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

		/// @brief 現在のフェンス値
		UINT64 fenceValue_ = 0;
		/// @brief フェンスオブジェクト
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
		/// @brief フェンスイベントハンドル
		HANDLE fenceEvent_ = nullptr;
	};
}
