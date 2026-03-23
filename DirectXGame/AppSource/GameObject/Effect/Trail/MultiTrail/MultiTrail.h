#pragma once
#include <vector>
#include <string>
#include <variant>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/RibbonTrail/RibbonTrail.h>
#include <GameObject/Effect/Trail/ShockwaveRingTrail/ShockwaveRingTrail.h>

class MultiTrail final
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		TrailPresetDataBank* presetData);

	// プリセット名で追加（例: "Axe_Ribbon"）
	void Add(const std::string& presetName);

	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }

	// 衝撃波等、任意発動型トリガー
	void Trigger(const std::string& presetName, const Vector3& position);

	void SetEnabled(bool enabled) { enabled_ = enabled; }

	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Clear();

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	TrailPresetDataBank* presetData_ = nullptr;

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = true;

	std::unordered_map<std::string, std::unique_ptr<RibbonTrail>> ribbonTrailCache_;
	std::unordered_map<std::string, std::unique_ptr<ShockwaveRingTrail>> shockwaveRingTrailCache_;

};