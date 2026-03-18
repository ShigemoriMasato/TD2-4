#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPresetTypes.h>

class RibbonTrail
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const RibbonTrailPreset& preset);

	/// @brief 紐づくモデルのワールド行列（ローカル2点をWSへ）
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }

	void SetEnabled(bool enabled) { enabled_ = enabled; }

	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

private:
	Trail trail_;
	RibbonTrailPreset preset_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = true;
};