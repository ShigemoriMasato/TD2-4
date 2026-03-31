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
		{0.0f, 100.0f},
		{0.0f, 200.0f},
		{0.0f, 300.0f}
	};

	Load();
	Sampling();
}

void LevelSystem::Update(float deltaTime) {
	if (end_ || stop_) return;
	if (waveVertices_.empty()) return;

	timer_ += deltaTime;
	castTime_ += deltaTime;

	currentPoint_ = GetPointFromTime(timer_ / 2.0f);

	float intensity = currentPoint_.y;
	//intensityに応じて敵を出す
	int enemyNum = static_cast<int>(intensity / 10.0f) + 1;
	float coolTime = 10.0f / intensity;
	coolTime = std::clamp(coolTime, 0.5f, 5.0f);

	if (castTime_ >= coolTime) {
		castTime_ = 0.0f;
		for (int i = 0; i < enemyNum; i++) {
			float x = std::uniform_real_distribution<float>(mapInfo_.minX, mapInfo_.maxX)(rng_);
			float z = std::uniform_real_distribution<float>(mapInfo_.minZ, mapInfo_.maxZ)(rng_);
			enemyManager_->PopEnemy({ x, 0.0f, z });
		}
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
			Sampling();
		}
	}

	ImGui::Text("Editing : %d/%d", editWaveIndex_ + 1, int(waveVertices_.size()));
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
	ImGui::SameLine();
	if (ImGui::Button("Erase")) {
		if (waveVertices_.size() > 4) {
			waveVertices_.erase(waveVertices_.begin() + editWaveIndex_);
		}
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
	binaryManager_.Boot(saveFilePath_);
	if (binaryManager_.IsEmpty()) {
		return;
	}
	int size = binaryManager_.Reverse<int>();
	waveVertices_.resize(size);
	for (int i = 0; i < size; i++) {
		waveVertices_[i].intensity = binaryManager_.Reverse<float>();
		waveVertices_[i].time = binaryManager_.Reverse<float>();
	}
}

void LevelSystem::Save() {
	binaryManager_.Boot(saveFilePath_);
	int size = (int)waveVertices_.size();
	binaryManager_.Register(&size);
	for (auto& vertex : waveVertices_) {
		binaryManager_.Register(&vertex.intensity);
		binaryManager_.Register(&vertex.time);
	}
	binaryManager_.Write(saveFilePath_);
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
	for (const auto& vertex : waveVertices_) {
		points.emplace_back(vertex.time, vertex.intensity);
	}

	Vector2 prev = MyMath::GetSplinePoint(points, 0.0f);

	lengthTable_[0] = 0.0f;

	for (int i = 1; i <= SAMPLE; i++) {
		float t = (float)i / SAMPLE;

		Vector2 cur = MyMath::GetSplinePoint(points, t);
		float dist = (cur - prev).Length();

		totalLength_ += dist;
		lengthTable_[i] = totalLength_;

		prev = cur;
	}
}

Vector2 LevelSystem::GetTFromDistance(float t) {
	std::vector<Vector2> points;
	points.reserve(waveVertices_.size());
	points.push_back({ waveVertices_.front().time, waveVertices_.front().intensity }); // 追加
	for(const auto& vertex : waveVertices_) {
		points.emplace_back(vertex.time, vertex.intensity);
	}
	points.push_back({ waveVertices_.back().time, waveVertices_.back().intensity }); // 追加

	// ---- ① ケアレスミス対策 ----
	if (points.size() < 4) {
		return { 0.0f, 0.0f };
	}

	t /= totalLength_; //距離 → 割合

	if (t <= 0.0f) return MyMath::GetSplinePoint(points, 0.0f);
	if (t >= 1.0f) return MyMath::GetSplinePoint(points, 1.0f);

	// ---- ② 割合 → 距離 ----
	float targetDistance = t * totalLength_;

	// ---- ③ 二分探索 ----
	int left = 0;
	int right = (int)lengthTable_.size() - 1;

	while (left < right) {
		int mid = (left + right) / 2;
		if (lengthTable_[mid] < targetDistance) {
			left = mid + 1;
		} else {
			right = mid;
		}
	}

	int idx = left;

	// ---- ④ 補間 ----
	if (idx == 0) return MyMath::GetSplinePoint(points, 0.0f);

	float l0 = lengthTable_[idx - 1];
	float l1 = lengthTable_[idx];

	float t0 = (float)(idx - 1) / (lengthTable_.size() - 1);
	float t1 = (float)(idx) / (lengthTable_.size() - 1);

	float ratio = (targetDistance - l0) / (l1 - l0);

	float internalT = t0 + (t1 - t0) * ratio;

	// ---- ⑤ 座標取得 ----
	return MyMath::GetSplinePoint(points, internalT);
}


Vector2 LevelSystem::GetPointFromTime(float time) {
	// ---- ① ケアレスミス ----
	if (waveVertices_.size() < 4 || totalLength_ <= 0.0f) {
		return { 0.0f, 0.0f };
	}

	const int SAMPLE = (int)lengthTable_.size() - 1;

	float prevDist = 0.0f;
	Vector2 prev = GetTFromDistance(0.0f);

	for (int i = 1; i <= SAMPLE; i++) {

		float dist = totalLength_ * ((float)i / SAMPLE);
		Vector2 cur = GetTFromDistance(dist);

		// ---- ② 区間に入ったか ----
		if ((prev.x <= time && time <= cur.x) ||
			(cur.x <= time && time <= prev.x)) {

			float dx = cur.x - prev.x;

			// ---- ③ xがほぼ同じ（縦）----
			if (fabs(dx) < 1e-5f) {
				return cur;
			}

			float ratio = (time - prev.x) / dx;

			Vector2 result;
			result.x = time;
			result.y = prev.y + (cur.y - prev.y) * ratio;

			return result;
		}

		prev = cur;
		prevDist = dist;
	}

	// ---- ④ 解なし（または複数の可能性）----
	return { 0.0f, 0.0f };
}