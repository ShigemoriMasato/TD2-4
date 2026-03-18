#pragma once
#include <vector>
#include <string>
#include <variant>

#include <GameObject/Effect/Trail/TrailPresetRepository/TrailPresetRepository.h>
#include <GameObject/Effect/Trail/RibbonTrail/RibbonTrail.h>
#include <GameObject/Effect/Trail/ShockwaveRingTrail/ShockwaveRingTrail.h>

class MultiTrail final
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		TrailPresetRepository* presetRepo);

	// プリセット名で追加（例: "Axe_Ribbon"）
	void AddFromPresetName(const std::string& presetName);

	// モデルに追従するタイプ用（Ribbon等）
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }

	// Shockwave等、任意発生型を外から叩けるようにしたい場合
	// まずは「全部」に投げる簡易版（activeでない個体は無視する実装でもOK）
	void TriggerShockwave(const Vector3& centerWS, const Vector3& normalWS);

	void SetEnabled(bool enabled) { enabled_ = enabled; }

	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Clear();

private:
	using TrailPtrVariant = std::variant<std::unique_ptr<RibbonTrail>, std::unique_ptr<ShockwaveRingTrail>>;

	struct Entry
	{
		std::string presetName;
		TrailPtrVariant trail;
	};

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	TrailPresetRepository* presetRepo_ = nullptr;

	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = true;

	std::vector<Entry> entries_;
};