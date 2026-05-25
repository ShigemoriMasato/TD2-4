#pragma once
#include <string>
#include <unordered_map>
#include <Assets/Texture/TextureManager.h>
#include <Scene/CommonData.h>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>

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


	// プリセット名で追加（例: "Axe_Ribbon"）
	int32_t Add(const std::string& presetName);
	// モデルに追従するタイプ用。モデルに追従してなくても使ってOK
	void SetModelWorld(const Matrix4x4& modelWorld);
	// 発生フラグをセット
	void SetEmittingFlag(const int32_t id, bool flag);
	// configをセット
	void SetConfig(const int32_t id, const TrailPresetVariant& presetVar);
	TrailPresetVariant GetConfig(const int32_t id);

private:
	SHEngine::TextureManager* textureManager_ = nullptr;
	TrailPresetDataBank* presetData_ = nullptr;
	TrailDrawer* drawer_ = nullptr;

	// TrailDrawerに登録
	void RegisterToDrawer();

	int32_t nextId_ = -1;
	std::unordered_map<int32_t, std::unique_ptr<ITrail>> trailCache_;
};