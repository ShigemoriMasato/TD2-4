#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>

class ShockwaveRingTrail
{
public:
	void Initialize(SHEngine::TextureManager* textureManager, const ShockwaveRingConfig& preset);
	void Update(float dt, const Matrix4x4& vpMatrix);

	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	void Trigger(const Vector3& center);
	void Stop();

	void Clear() { trail_.Clear(); }

	bool IsActive() const { return active_; }

	Trail& GetTrail() { return trail_; }
	const Trail& GetTrail() const { return trail_; }

private:
	static Vector3 NormalizeSafe(const Vector3& v);
	static Vector3 Cross(const Vector3& a, const Vector3& b);
	static float Hash01(int i);

private:
	Trail trail_;
	ShockwaveRingConfig preset_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };

	bool active_ = false;
	float time_ = 0.0f;
	Vector3 position_{};
};