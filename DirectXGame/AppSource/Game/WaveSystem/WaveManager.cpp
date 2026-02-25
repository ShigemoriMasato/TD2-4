#include "WaveManager.h"
#include <imgui/imgui.h>

void WaveManager::Initialize(float waveTime, float waitTime) {
	wave_.Initialize(waveTime, waitTime);

	// コールバックを設定
	wave_.SetOnWaveStartCallback([this](uint32_t waveNumber) {
		OnWaveStart(waveNumber);
	});

	wave_.SetOnWaveEndCallback([this](uint32_t waveNumber) {
		OnWaveEnd(waveNumber);
	});
}

void WaveManager::Update(float deltaTime) {
	wave_.Update(deltaTime);
}

void WaveManager::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Wave System");

	// 現在のWave番号を表示
	ImGui::Text("Current Wave: %u", wave_.GetCurrentWave());

	// 現在の状態を表示
	const char* stateText = "";
	switch (wave_.GetWaveState()) {
	case WaveState::Ready:
		stateText = "Ready (Waiting to Start)";
		break;
	case WaveState::InProgress:
		stateText = "In Progress";
		break;
	case WaveState::Complete:
		stateText = "Complete (Waiting for Next Wave)";
		break;
	}
	ImGui::Text("State: %s", stateText);

	// 残り時間を表示
	ImGui::Text("Remaining Time: %.1f sec", wave_.GetRemainingTime());

	// プログレスバーを表示
	float progress = 0.0f;
	if (wave_.GetWaveState() == WaveState::InProgress) {
		progress = 1.0f - (wave_.GetRemainingTime() / wave_.GetWaveTime());
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Wave Progress");
	} else {
		progress = 1.0f - (wave_.GetRemainingTime() / wave_.GetWaitTime());
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Wait Time");
	}

	ImGui::Separator();

	// Wave設定
	float waveTime = wave_.GetWaveTime();
	if (ImGui::DragFloat("Wave Time", &waveTime, 1.0f, 1.0f, 300.0f)) {
		wave_.SetWaveTime(waveTime);
	}

	float waitTime = wave_.GetWaitTime();
	if (ImGui::DragFloat("Wait Time", &waitTime, 1.0f, 1.0f, 60.0f)) {
		wave_.SetWaitTime(waitTime);
	}

	ImGui::Separator();

	// 手動操作ボタン
	if (wave_.IsWaveReady() && ImGui::Button("Start Wave Manually")) {
		wave_.StartWave();
	}

	if (wave_.IsWaveInProgress() && ImGui::Button("End Wave Manually")) {
		wave_.EndWave();
	}

	if (ImGui::Button("Reset to Wave 1")) {
		wave_.Reset();
	}

	ImGui::End();
#endif
}

void WaveManager::OnWaveStart(uint32_t waveNumber) {
	// Wave開始時の処理
	// ここで敵のスポーンなどを行う
	// 例: EnemyManagerに敵の生成を依頼
}

void WaveManager::OnWaveEnd(uint32_t waveNumber) {
	// Wave終了時の処理
	// ここで残った敵を処理したり、報酬を与えたりする
}
