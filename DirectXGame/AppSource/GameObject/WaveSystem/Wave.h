#pragma once
#include <cstdint>
#include <functional>

/// @brief Waveの状態
enum class WaveState {
	Ready,      // Wave開始前の待機状態
	InProgress, // Wave実行中
	Complete,   // Wave終了後の待機状態
};


// Wave開始 -> Wave実行 -> Wave終了 -> 待機 -> Wave再開始のループを管理する

// @brief ウェーブシステムを管理するクラス
class Wave {
public:

	Wave() = default;
	~Wave() = default;

	// @brief Waveシステムを初期化
	// @param waveTime 各Waveの実行時間（秒）
	// @param waitTime Wave終了後の待機時間（秒）
	void Initialize(float waveTime = 30.0f, float waitTime = 10.0f);

	// @brief Waveシステムを更新
	// @param deltaTime デルタタイム（秒）
	void Update(float deltaTime);

	// @brief 現在のWave番号を取得
	// @return Wave番号（1から開始）
	uint32_t GetCurrentWave() const { return currentWave_; }

	// @brief 現在のWave状態を取得
	// @return Wave状態
	WaveState GetWaveState() const { return waveState_; }

	// @brief 現在のタイマー値を取得（残り時間）
	// @return 残り時間（秒）
	float GetRemainingTime() const { return currentTimer_; }

	// @brief Wave実行中かどうかを確認
	// @return Wave実行中ならtrue
	bool IsWaveInProgress() const { return waveState_ == WaveState::InProgress; }

	// @brief Wave完了（待機）状態かどうかを確認
	// @return Wave完了状態ならtrue
	bool IsWaveComplete() const { return waveState_ == WaveState::Complete; }

	// @brief Wave開始待機状態かどうかを確認
	// @return Wave開始待機状態ならtrue
	bool IsWaveReady() const { return waveState_ == WaveState::Ready; }

	// @brief Waveを強制的に開始する
	void StartWave();

	// @brief Waveを強制的に終了する
	void EndWave();

	// @brief Waveシステムをリセット（Wave1からやり直し）
	void Reset();

	// @brief Wave開始時のコールバックを設定
	// @param callback コールバック関数（引数: Wave番号）
	void SetOnWaveStartCallback(std::function<void(uint32_t)> callback) {
		onWaveStartCallback_ = callback;
	}

	// @brief Wave終了時のコールバックを設定
	// @param callback コールバック関数（引数: Wave番号）
	void SetOnWaveEndCallback(std::function<void(uint32_t)> callback) {
		onWaveEndCallback_ = callback;
	}

	// @brief Wave時間を設定
	// @param waveTime Wave実行時間（秒）
	void SetWaveTime(float waveTime) { waveTime_ = waveTime; }

	// @brief 待機時間を設定
	// @param waitTime Wave完了後の待機時間（秒）
	void SetWaitTime(float waitTime) { waitTime_ = waitTime; }

	// @brief Wave時間を取得
	// @return Wave実行時間（秒）
	float GetWaveTime() const { return waveTime_; }

	// @brief 待機時間を取得
	// @return Wave完了後の待機時間（秒）
	float GetWaitTime() const { return waitTime_; }

private:

	/// @brief 現在のWave番号（1から開始）
	uint32_t currentWave_ = 1;

	/// @brief 現在のWave状態
	WaveState waveState_ = WaveState::Ready;

	/// @brief 現在のタイマー（カウントダウン）
	float currentTimer_ = 0.0f;

	/// @brief Wave実行時間（秒）
	float waveTime_ = 30.0f;

	/// @brief Wave完了後の待機時間（秒）
	float waitTime_ = 10.0f;

	/// @brief Wave開始時のコールバック
	std::function<void(uint32_t)> onWaveStartCallback_;

	/// @brief Wave終了時のコールバック
	std::function<void(uint32_t)> onWaveEndCallback_;

};