#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPresetTypes.h>

class ShockwaveRingTrail
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const ShockwaveRingPreset& preset);

	/// @brief 発生（center + normalで平面指定）
	void Trigger(const Vector3& centerWS, const Vector3& normalWS);

	void Stop();

	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	bool IsActive() const { return active_; }

private:
	static Vector3 NormalizeSafe_(const Vector3& v);
	static Vector3 Cross_(const Vector3& a, const Vector3& b);
	static float Hash01_(int i);

private:
	Trail trail_;
	ShockwaveRingPreset preset_{};

	bool active_ = false;
	float time_ = 0.0f;

	Vector3 centerWS_{};
	Vector3 normalWS_{ 0.0f, 1.0f, 0.0f };
};