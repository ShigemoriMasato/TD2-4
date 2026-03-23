#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Utility/Matrix.h>
#include <Utility/Vector.h>

class ParticlePresetDataBank;
class Particle;

class MultiParticle final
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		ParticlePresetDataBank* presetData);

	// プリセット名で追加（例:"testParticle"）
	void Add(const std::string& presetName);

	void Trigger(const std::string& presetName, const Vector3& position);
	void Stop(const std::string& presetName);

	void SetEnabled(bool enabled) { enabled_ = enabled; }

	void Update(float dt);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

	void Clear();

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	ParticlePresetDataBank* presetData_ = nullptr;

	bool enabled_ = true;

	std::unordered_map<std::string, std::unique_ptr<Particle>> cache_;
};