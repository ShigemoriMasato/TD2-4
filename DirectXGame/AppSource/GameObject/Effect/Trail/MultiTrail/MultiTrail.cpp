#include "MultiTrail.h"
#include <GameObject/Effect/Trail/Drawer/TrailDrawer.h>
#include <stdexcept>

void MultiTrail::Initialize(
	SHEngine::TextureManager* textureManager,
	TrailPresetDataBank* presetData)
{
	textureManager_ = textureManager;
	presetData_ = presetData;

	nextId_ = -1;

	ribbonTrailCache_.clear();
	shockwaveRingTrailCache_.clear();
	enabled_ = true;
	modelWorld_ = Matrix4x4::Identity();
}

int32_t MultiTrail::Add(const std::string& presetName)
{
	if (!presetData_) return -1;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	nextId_++;

	// トレイルタイプごとに生成
	if (std::holds_alternative<RibbonTrailConfig>(presetVar))
	{
		const auto& preset = std::get<RibbonTrailConfig>(presetVar);
		auto trail = std::make_unique<RibbonTrail>();
		trail->Initialize(textureManager_, preset);
		trail->SetModelWorld(modelWorld_);
		ribbonTrailCache_[nextId_] = std::move(trail);
	}
	else if (std::holds_alternative<ShockwaveRingConfig>(presetVar))
	{
		const auto& preset = std::get<ShockwaveRingConfig>(presetVar);
		auto trail = std::make_unique<ShockwaveRingTrail>();
		trail->Initialize(textureManager_, preset);
		trail->SetModelWorld(modelWorld_);
		shockwaveRingTrailCache_[nextId_] = std::move(trail);
	}

	return nextId_;
}

void MultiTrail::SetEmittingFlag(const int32_t id, bool flag)
{
	if (ribbonTrailCache_.count(id))
	{
		ribbonTrailCache_.at(id)->SetEnabled(flag);
	}
	else if (shockwaveRingTrailCache_.count(id))
	{

	}
}

void MultiTrail::Clear()
{
	for (auto& [name, trail] : ribbonTrailCache_)
	{
		trail->Clear();
	}

	for (auto& [name, trail] : shockwaveRingTrailCache_)
	{
		trail->Clear();
	}
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

void MultiTrail::RegisterToDrawer(TrailDrawer* drawer)
{
	if (!drawer) return;

	for (auto& [id, t] : ribbonTrailCache_)
	{
		drawer->Register(&t->GetTrail());
	}
	for (auto& [id, t] : shockwaveRingTrailCache_)
	{
		drawer->Register(&t->GetTrail());
	}
}