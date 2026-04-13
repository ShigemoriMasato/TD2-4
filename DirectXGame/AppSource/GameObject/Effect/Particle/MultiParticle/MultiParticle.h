#pragma once
#include <string>
#include <unordered_map>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>

#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/Type/FountainParticle/FountainParticle.h>

class ParticleDrawer;

class MultiParticle final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		ParticlePresetDataBank* presetData);
	void Update(float dt);
	void Clear();

	// プリセット名で追加（例: "Axe_Ribbon"）
	int32_t Add(const std::string& presetName);
	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);
	void SetEmittingFlag(bool flag) { enabled_ = flag; }
	// configをセット
	void SetConfig(const int32_t id, const ParticlePresetVariant& presetVar);
	ParticlePresetVariant GetConfig(const int32_t id);

	// ParticleDrawerに登録
	void RegisterToDrawer(ParticleDrawer* drawer);

	std::vector<Matrix4x4> GetParticleWorlds(const int32_t id);
	size_t GetAliveCount(const int32_t id) const;

private:
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	ParticlePresetDataBank* presetData_ = nullptr;

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = false;

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<FountainParticle>> fountainCache_;
	//std::unordered_map<int32_t, std::unique_ptr<GoToTargetParticle>> goToTargetCache_;
};