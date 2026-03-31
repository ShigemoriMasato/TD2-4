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

	// プリセット名で追加（例:"testParticle"）
	void Add(const std::string& presetName);

	void Trigger(const std::string& presetName, const Vector3& position);
	void Stop(const std::string& presetName);

	std::vector<Matrix4x4> GetParticleWorlds(const std::string& presetName);

	void SetEnabled(bool enabled) { enabled_ = enabled; }

	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Clear();

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	ParticlePresetDataBank* presetData_ = nullptr;

	bool enabled_ = true;

	std::unordered_map<std::string, std::unique_ptr<FountainParticle>> fountainCache_;
};