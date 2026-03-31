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

	float dist = GetTFromDistance(timer_);


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

	if (ImGui::Button("Add")) {
		waveVertices_.push_back({ 0.0f, 10.0f });
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

void LevelSystem::Sampling() {
	constexpr int SAMPLE = 1000;
	lengthTable_.resize(SAMPLE + 1);

	std::vector<Vector2> points;
	for(const auto& vertex : waveVertices_) {
		points.emplace_back(vertex.time, vertex.intensity);
	}

	float totalLength = 0.0f;
	Vector2 prev = MyMath::GetSplinePoint(points, 0.0f);

	lengthTable_[0] = 0.0f;

	for (int i = 1; i <= SAMPLE; i++) {
		float t = (float)i / SAMPLE;

		Vector2 cur = MyMath::GetSplinePoint(points, t);
		float dist = (cur - prev).Length();

		totalLength += dist;
		lengthTable_[i] = totalLength;

		prev = cur;
	}
}

float LevelSystem::GetTFromDistance(float distance) {
	float target = distance;
	auto& table = lengthTable_;

	int left = 0;
	int right = (int)lengthTable_.size() - 1;

	// 二分探索
	while (left < right) {
		int mid = (left + right) / 2;
		if (table[mid] < target) left = mid + 1;
		else right = mid;
	}

	int idx = left;

	// 補間
	float l0 = table[idx - 1];
	float l1 = table[idx];

	float t0 = (float)(idx - 1) / (table.size() - 1);
	float t1 = (float)(idx) / (table.size() - 1);

	float ratio = (target - l0) / (l1 - l0);

	return t0 + (t1 - t0) * ratio;
}
