#pragma once
#include <string>
#include <unordered_map>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <Scene/CommonData.h>

#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/Type/FountainParticle/FountainParticle.h>
#include <GameObject/Effect/Particle/Type/GoToTargetParticle/GoToTargetParticle.h>
#include <GameObject/Effect/Particle/Type/BillboardScaleParticle/BillboardScaleParticle.h>
#include <GameObject/Effect/Particle/Type/BillboardColorParticle/BillboardColorParticle.h>

class ParticleDrawer;

class MultiParticle final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		CommonData* commonData);
	void Update(float dt);
	void Draw();
	void Clear();

	// プリセット名で追加（例: "Axe_Ribbon"）
	int32_t Add(const std::string& presetName);
	// カメラ位置をセット。Billboard等のカメラ座標が必要なParticleを利用する場合のみ必要。
	void SetCameraPos(const Vector3& cameraPos);
	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld);
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);
	// configをセット
	void SetConfig(const int32_t id, const ParticlePresetVariant& presetVar);
	ParticlePresetVariant GetConfig(const int32_t id);


	std::vector<Matrix4x4> GetParticleWorlds(const int32_t id);
	size_t GetAliveCount(const int32_t id) const;

private:
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	ParticlePresetDataBank* presetData_ = nullptr;
	ParticleDrawer* drawer_ = nullptr;

	// ParticleDrawerに登録
	void RegisterToDrawer();

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<PhysicsParticle>> physicsCache_;
	//std::unordered_map<int32_t, std::unique_ptr<OnTrailParticle>> onTrailCache_;
	std::unordered_map<int32_t, std::unique_ptr<GoToTargetParticle>> goToTargetCache_;
	std::unordered_map<int32_t, std::unique_ptr<BillboardScaleParticle>> billboardScaleCache_;
	//std::unordered_map<int32_t, std::unique_ptr<BillboardScale2Particle>> billboardScale2Cache_;
	std::unordered_map<int32_t, std::unique_ptr<BillboardColorParticle>> billboardColorCache_;
};