#include "Wave.h"

void Wave::Initialize() {
	currentWave_ = 1;
	waveState_ = WaveState::Ready;
	currentTimer_ = waitTime_;
}

void Wave::Update(float deltaTime) {
	switch (waveState_) {
	case WaveState::Ready:
		// 待機状態：タイマーをカウントダウン
		currentTimer_ -= deltaTime;
		if (currentTimer_ <= 0.0f) {
			// タイマーが0になったらWave開始
			StartWave();
		}
		break;

	case WaveState::InProgress:
		// Wave実行中：タイマーをカウントダウン
		currentTimer_ -= deltaTime;
		if (currentTimer_ <= 0.0f) {
			// タイマーが0になったらWave終了
			EndWave();
		}
		break;

	case WaveState::Complete:
		// Wave完了後の待機状態：タイマーをカウントダウン
		currentTimer_ -= deltaTime;
		if (currentTimer_ <= 0.0f) {
			// タイマーが0になったら次のWaveへ
			currentWave_++;
			waveState_ = WaveState::Ready;
			currentTimer_ = waitTime_;
		}
		break;
	}
}

void Wave::StartWave() {
	waveState_ = WaveState::InProgress;
	currentTimer_ = waveTime_;

	// コールバックを実行
	if (onWaveStartCallback_) {
		onWaveStartCallback_(currentWave_);
	}
}

void Wave::EndWave() {
	waveState_ = WaveState::Complete;
	currentTimer_ = waitTime_;

	// コールバックを実行
	if (onWaveEndCallback_) {
		onWaveEndCallback_(currentWave_);
	}
}

void Wave::Reset() {
	currentWave_ = 1;
	waveState_ = WaveState::Ready;
	currentTimer_ = waitTime_;
}
