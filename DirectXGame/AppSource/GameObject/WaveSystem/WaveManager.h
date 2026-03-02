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
	void Initialize(float waveTime = 30.0f, float waitTime = 10.0f);

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
