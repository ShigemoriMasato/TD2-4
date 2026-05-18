#pragma once
#include <unordered_map>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/TrailOnParticle.h>


struct MultiParticleData
{
	MultiParticle multiParticle;
	// 一回のエミットで削除するか
	bool oneShot = false;
	// 既に発生したことがあるか（oneShot用）
	bool emittedOnce = false;
};

struct MultiTrailData
{
	MultiTrail multiTrail;
	// 一回のエミットで削除するか
	bool oneShot = false;
	// 既に発生したことがあるか（oneShot用）
	bool emittedOnce = false;
};

struct TrailOnParticleData
{
	TrailOnParticle trailOnParticle;
	// 一回のエミットで削除するか
	bool oneShot = false;
	// 既に発生したことがあるか（oneShot用）
	bool emittedOnce = false;
};


class EnemyEffect
{
public:
	EnemyEffect();
	~EnemyEffect();

	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		CommonData* commonData);
	void Update(float dt);
	void Draw();

	void SetCameraPos(Vector3 pos);

	void CreateDeathEffect1(const Matrix4x4& world);

private:
	SHEngine::TextureManager* textureManager_;
	SHEngine::ModelManager* modelManager_;
	CommonData* commonData_;


	std::unordered_map<int, MultiParticleData> multiParticles_;
	int nextMultiParticleId_ = 0;
	std::unordered_map<int, MultiTrailData> multiTrails_;
	int nextMultiTrailId_ = 0;
	std::unordered_map<int, TrailOnParticleData> trailOnParticle_;
	int nextTrailOnParticleId_ = 0;
};

