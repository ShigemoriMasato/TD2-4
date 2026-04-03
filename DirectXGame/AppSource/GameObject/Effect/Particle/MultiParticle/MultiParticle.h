#pragma once
#include <string>
#include <unordered_map>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>

#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/Type/FountainParticle/FountainParticle.h>

class MultiParticle final
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		ParticlePresetDataBank* presetData);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);
	void Clear();

	// プリセット名で追加（例:"testParticle"）
	int32_t Add(const std::string& presetName);
	// 発生位置をセット
	void SetEmitPos(const int32_t id, const Vector3& pos);
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);

	std::vector<Matrix4x4> GetParticleWorlds(const int32_t id);
	size_t GetAliveCount(const int32_t id) const;



private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	ParticlePresetDataBank* presetData_ = nullptr;

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<FountainParticle>> fountainCache_;
};