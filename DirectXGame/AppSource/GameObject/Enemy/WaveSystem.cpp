#include "WaveSystem.h"
#include <imgui/imgui.h>

WaveSystem::~WaveSystem() {
	if(isSave_) {
		Save();
	}
}

void WaveSystem::Initialize(EnemyManager* enemyManager, int stageNum, float minX, float maxX, float minZ, float maxZ) {
	enemyManager_ = enemyManager;
	stageNum_ = stageNum;
	std::string fileName = "Stage" + std::to_string(stageNum_) + ".bytes";
	timer_ = 0.0f;
	isActive_ = true;

	Load();
	if(waveConfigs_.size() <= stageNum_) {
		waveConfigs_.resize(stageNum_ + 1);
		Adjust();
	}

	// 座標範囲の設定
	minX_ = minX;
	maxX_ = maxX;
	minZ_ = minZ;
	maxZ_ = maxZ;
}

void WaveSystem::Update(float deltaTime) {
	if (!isActive_) return;
	timer_ += deltaTime;

	const auto& config = waveConfigs_[stageNum_];
	if (timer_ > config.spawnInterval) {
		std::pair<int, int> spawnRange = { std::max(2, int(config.enemyCount - 1)), std::min(2, int(config.enemyCount / 4)) }; // スポーンするX座標の範囲
		std::uniform_real_distribution<float> spawnDist(static_cast<float>(spawnRange.second), static_cast<float>(spawnRange.first));
		std::uniform_real_distribution<float> xDist(minX_, maxX_); // X座標の範囲
		std::uniform_real_distribution<float> zDist(minZ_, maxZ_); // Z座標の範囲

		int spawnNum = static_cast<int>(spawnDist(rng_));

		for (int i = 0; i < spawnNum; ++i) {
			Vector3 initPos = { xDist(rng_), 0.0f, zDist(rng_) };
			enemyManager_->PopEnemy(initPos, static_cast<int>(config.enemyHp));
		}

		timer_ = 0.0f;
	}
}

void WaveSystem::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("WaveSystem");

	ImGui::Checkbox("Save", &isSave_);		//閉じたときにセーブするかどうか

	ImGui::DragFloat("IncreaseIntercal", &increaseIntercal_, 0.01f, 0.0f);
	ImGui::DragFloat("IncreaseEnemyCount", &increaseEnemyCount_, 0.01f, 0.0f);
	ImGui::DragFloat("IncreaseHp", &increaseHp_, 0.1f, 0.0f);

	if (ImGui::Button("Adjust")) {
		Adjust();
	}

	for(int i = 0; i < waveConfigs_.size(); ++i) {
		auto& config = waveConfigs_[i];
		ImGui::PushID(i);
		ImGui::Separator();
		ImGui::Checkbox("Adjust", &config.isAdjust);
		ImGui::DragFloat("Spawn Interval", &config.spawnInterval, 0.1f, 0.1f);
		ImGui::DragFloat("Enemy Count", &config.enemyCount, 0.1f, 1);
		ImGui::DragFloat("Enemy HP", &config.enemyHp, 0.5f, 1.0f);
		ImGui::PopID();
	}

	ImGui::End();
#endif
}

void WaveSystem::Adjust() {
	for (int i = 1; i < waveConfigs_.size(); ++i) {
		auto& preConfig = waveConfigs_[i - 1];
		auto& config = waveConfigs_[i];

		if (!config.isAdjust) {
			continue;
		}

		config.spawnInterval = preConfig.spawnInterval - increaseIntercal_;
		config.enemyCount = preConfig.enemyCount + increaseEnemyCount_;
		config.enemyHp = preConfig.enemyHp + increaseHp_;
	}
}

void WaveSystem::Load() {
}

void WaveSystem::Save() {
}
