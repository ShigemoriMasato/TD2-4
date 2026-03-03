#pragma once
#include "Wave.h"
#include <memory>

// @brief Waveシステムを管理し、敵のスポーンなどのゲームロジックと連携するクラス
class WaveManager {
public:

	WaveManager() = default;
	~WaveManager() = default;

	// @brief WaveManagerを初期化
	// @param waveTime 各Waveの実行時間（秒）
	// @param waitTime Wave終了後の待機時間（秒）
	void Initialize();

	// @brief WaveManagerを更新
	// @param deltaTime デルタタイム（秒）
	void Update(float deltaTime);

	// @brief ImGuiでデバッグ情報を表示
	void DrawImGui();

	// @brief Waveインスタンスを取得
	// @return Wave参照
	Wave& GetWave() { return wave_; }

	// @brief Wave番号を取得
	// @return 現在のWave番号
	uint32_t GetCurrentWave() const { return wave_.GetCurrentWave(); }

	// @brief Waveをリセット
	void Reset() { wave_.Reset(); }

	// @brief 実行中のWaveが終了したかを確認
	// @return Wave終了（Complete状態）ならtrue
	bool IsWaveComplete() const { return wave_.IsWaveComplete(); }

	// @brief Waveが実行中かを確認
	// @return Wave実行中（InProgress状態）ならtrue
	bool IsWaveInProgress() const { return wave_.IsWaveInProgress(); }

	// @brief Waveが開始待機中かを確認
	// @return Wave開始待機中（Ready状態）ならtrue
	bool IsWaveReady() const { return wave_.IsWaveReady(); }

private:

	// @brief Wave開始時のコールバック
	// @param waveNumber Wave番号
	void OnWaveStart(uint32_t waveNumber);

	// @brief Wave終了時のコールバック
	// @param waveNumber Wave番号
	void OnWaveEnd(uint32_t waveNumber);

private:

	/// @brief Waveシステム
	Wave wave_;
};
