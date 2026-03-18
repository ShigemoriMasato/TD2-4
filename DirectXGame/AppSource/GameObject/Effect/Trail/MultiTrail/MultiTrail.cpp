#include "MultiTrail.h"
#include <stdexcept>

void MultiTrail::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	TrailPresetDataBank* presetRepo)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	presetData_ = presetRepo;

	ribbonTrailCache_.clear();
	shockwaveRingTrailCache_.clear();
	enabled_ = true;
	modelWorld_ = Matrix4x4::Identity();
}

void MultiTrail::AddFromPresetName(const std::string& presetName)
{
	if (!presetData_) return;

	// プリセットデータを取得(トレイルタイプは分類されていない)
	const auto& presetVar = presetData_->Get(presetName);

	// トレイルタイプごとに生成
	if (std::holds_alternative<RibbonTrailPreset>(presetVar))
	{
		const auto& preset = std::get<RibbonTrailPreset>(presetVar);
		auto trail = std::make_unique<RibbonTrail>();
		trail->Initialize(drawDataManager_, textureManager_, preset);
		trail->SetModelWorld(modelWorld_);
		ribbonTrailCache_[presetName] = std::move(trail);
	}
	else if (std::holds_alternative<ShockwaveRingPreset>(presetVar))
	{
		const auto& preset = std::get<ShockwaveRingPreset>(presetVar);
		auto trail = std::make_unique<ShockwaveRingTrail>();
		trail->Initialize(drawDataManager_, textureManager_, preset);
		trail->SetModelWorld(modelWorld_);
		shockwaveRingTrailCache_[presetName] = std::move(trail);
	}
}

void MultiTrail::Trigger(const std::string& presetName, const Vector3& position)
{
	shockwaveRingTrailCache_.at(presetName)->Trigger(position);
}

void MultiTrail::Clear()
{
	ribbonTrailCache_.clear();
	shockwaveRingTrailCache_.clear();
}

void MultiTrail::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (!enabled_) return;

	for (auto& [name, trail] : ribbonTrailCache_)
	{
		trail->SetModelWorld(modelWorld_);
		trail->Update(dt, vpMatrix);
	}

	for (auto& [name, trail] : shockwaveRingTrailCache_)
	{
		trail->SetModelWorld(modelWorld_);
		trail->Update(dt, vpMatrix);
	}
}

void MultiTrail::Draw(CmdObj* cmdObj)
{
	if (!enabled_) return;

	for (auto& [name, trail] : ribbonTrailCache_)
	{
		trail->Draw(cmdObj);
	}
	for (auto& [name, trail] : shockwaveRingTrailCache_)
	{
		trail->Draw(cmdObj);
	}
}