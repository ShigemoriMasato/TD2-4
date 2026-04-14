#pragma once
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include <Scene/CommonData.h>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/Type/RibbonTrail/RibbonTrail.h>
#include <GameObject/Effect/Trail/Type/ShockwaveRingTrail/ShockwaveRingTrail.h>

class TrailDrawer;

class MultiTrail final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		CommonData* commonData);
	void Update(float dt);
	void Draw();

	// すべてのトレイルデータをクリア
	void Clear();
	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }

	// プリセット名で追加（例: "Axe_Ribbon"）
	int32_t Add(const std::string& presetName);
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);

private:
	// TrailDrawerに登録
	void RegisterToDrawer();

	SHEngine::TextureManager* textureManager_ = nullptr;
	TrailPresetDataBank* presetData_ = nullptr;
	TrailDrawer* drawer_ = nullptr;

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<RibbonTrail>> ribbonTrailCache_;
	std::unordered_map<int32_t, std::unique_ptr<ShockwaveRingTrail>> shockwaveRingTrailCache_;
};