#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

/**
 * @class FenceManager
 * @brief DirectX12のフェンスを使用したGPU同期を管理するクラス
 * 
 * CPUとGPUの同期を取るためのフェンス機能を提供する。
 * GPUの処理完了を待機したり、特定のコマンド完了を確認したりする機能を持つ。
 */
class FenceManager {
public:

	/// @brief デフォルトコンストラクタ
	FenceManager() = default;
	
	/// @brief デストラクタ
	~FenceManager() = default;

	/**
	 * @brief フェンスマネージャーを初期化する
	 * @param queue コマンドキュー
	 * @param device DirectX12デバイス
	 */
	void Initialize(ID3D12CommandQueue* queue, ID3D12Device* device);

	/**
	 * @brief GPUにシグナルを送信する
	 * 
	 * コマンドキューにフェンス値の更新を要求し、GPU処理の完了を追跡可能にする。
	 */
	void SendSignal();

	/**
	 * @brief GPUシグナルの完了を確認する
	 * @param offset フェンス値のオフセット（デフォルト: 0）
	 * @return true: シグナル完了、false: 処理中
	 */
	bool SignalChecker(int offset = 0);
	
	/**
	 * @brief GPUの処理が完全に完了するまで待機する
	 * 
	 * 全てのGPUコマンドの実行完了を待つブロッキング処理。
	 */
	void WaitForGPU();

	/**
	 * @brief フェンスリソースを解放する
	 */
	void Finalize();

	/**
	 * @brief 現在のフェンス値を取得する
	 * @return フェンス値
	 */
	UINT64 GetCurrentFenceValue() const { return fenceValue_; }

private:

	/// @brief コマンドキューへのポインタ
	ID3D12CommandQueue* commandQueue_ = nullptr;

	/// @brief 現在のフェンス値
	UINT64 fenceValue_ = 0;
	/// @brief フェンスオブジェクト
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	/// @brief フェンスイベントハンドル
	HANDLE fenceEvent_ = nullptr;
};
