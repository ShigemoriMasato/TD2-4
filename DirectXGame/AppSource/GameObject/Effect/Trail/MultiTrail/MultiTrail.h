#pragma once
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/Type/RibbonTrail/RibbonTrail.h>
#include <GameObject/Effect/Trail/Type/ShockwaveRingTrail/ShockwaveRingTrail.h>

class TrailDrawer;

class MultiTrail final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		TrailPresetDataBank* presetData);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Clear();

	// プリセット名で追加（例: "Axe_Ribbon"）
	int32_t Add(const std::string& presetName);
	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);
	void SetEmittingFlag(bool flag) { enabled_ = flag; }

	// TrailDrawerに登録
	void RegisterToDrawer(TrailDrawer* drawer);


private:
	SHEngine::TextureManager* textureManager_ = nullptr;
	TrailPresetDataBank* presetData_ = nullptr;

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = false;

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<RibbonTrail>> ribbonTrailCache_;
	std::unordered_map<int32_t, std::unique_ptr<ShockwaveRingTrail>> shockwaveRingTrailCache_;

};