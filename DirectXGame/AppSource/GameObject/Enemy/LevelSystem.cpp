#include "LevelSystem.h"
#include <imgui/imgui.h>

LevelSystem::~LevelSystem() {
	Save();
}

void LevelSystem::Initialize(EnemyManager* enemyManager, Vector3* playerPosPtr, const MapInfo& mapInfo) {
	enemyManager_ = enemyManager;
	// 座標範囲の設定
	mapInfo_ = mapInfo;
	playerPosPtr_ = playerPosPtr;

	waveVertices_ = {
		{0.0f, 0.0f},
		{0.0f, 300.0f}
	};

	Load();
}

void LevelSystem::Update(float deltaTime) {
	if (end_ || stop_) return;
	if (waveVertices_.empty()) return;

	timer_ += deltaTime;

	float intensity = waveVertices_[currentWaveIndex_].intensity;
	//このintensityに基づいて敵を生成する

	if (waveVertices_[currentWaveIndex_ + 1].time <= timer_) {
		currentWaveIndex_++;
	}

	if (currentWaveIndex_ >= waveVertices_.size() - 1) {
		//全てのWaveを超過した
		end_ = true;
	}
}

void LevelSystem::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Level System Editor");

	if (ImGui::Checkbox("Stop", &stop_)) {
		//停止状態を解除したらソートする
		if (!stop_) {
			Sort();
		}
	}

	ImGui::Text("Editing : %d", editWaveIndex_);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		editWaveIndex_--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		editWaveIndex_++;
	}

	if (waveVertices_.empty()) {
		ImGui::End();
		return;
	}

	editWaveIndex_ = std::clamp(editWaveIndex_, 0, static_cast<int>(waveVertices_.size() - 1));

	if (ImGui::DragFloat("Intensity", &waveVertices_[editWaveIndex_].intensity, 0.1f)) {
		//intensityを変更したときの処理
	}

	if (ImGui::DragFloat("Time", &waveVertices_[editWaveIndex_].time, 0.1f)) {
		//timeを変更したときの処理
	}

	ImGui::End();

#endif
}

void LevelSystem::Load() {
}

void LevelSystem::Save() {
}

void LevelSystem::Sort() {
	std::sort(waveVertices_.begin(), waveVertices_.end(),
		[](const WaveVertex& a, const WaveVertex& b) {
			return a.time < b.time;
		});
}
