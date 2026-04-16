#include "MultiTrail.h"
#include <GameObject/Effect/Trail/Drawer/TrailDrawer.h>
#include <Scene/CommonData.h>
#include <stdexcept>


void MultiTrail::Initialize(SHEngine::TextureManager* textureManager, CommonData* commonData)
{
	textureManager_ = textureManager;
	presetData_ = &commonData->trailPresetDataBank;
	drawer_ = commonData->trailDrawer.get();

	nextId_ = -1;

	ribbonTrailCache_.clear();
	shockwaveRingTrailCache_.clear();
	perTrailModelWorld_.clear();
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
		ribbonTrailCache_[nextId_] = std::move(trail);
	}
	else if (std::holds_alternative<ShockwaveRingConfig>(presetVar))
	{
		const auto& preset = std::get<ShockwaveRingConfig>(presetVar);
		auto trail = std::make_unique<ShockwaveRingTrail>();
		trail->Initialize(textureManager_, preset);
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
	for (auto& [id, trail] : ribbonTrailCache_)
	{
		trail->Clear();
	}

	for (auto& [id, trail] : shockwaveRingTrailCache_)
	{
		trail->Clear();
	}
}

void MultiTrail::SetModelWorld(int32_t id, const Matrix4x4& modelWorld)
{
	perTrailModelWorld_[id] = modelWorld;
}

void MultiTrail::Update(float dt)
{
	for (auto& [id, trail] : ribbonTrailCache_)
	{
		Matrix4x4 world = modelWorld_;
		auto pit = perTrailModelWorld_.find(id);
		if (pit != perTrailModelWorld_.end()) world = world * pit->second;

		trail->SetModelWorld(world);
		trail->Update(dt);
	}

	for (auto& [id, trail] : shockwaveRingTrailCache_)
	{
		Matrix4x4 world = modelWorld_;
		auto pit = perTrailModelWorld_.find(id);
		if (pit != perTrailModelWorld_.end()) world = world * pit->second;

		trail->SetModelWorld(world);
		trail->Update(dt);
	}
}

void MultiTrail::Draw()
{
	RegisterToDrawer();
}

void MultiTrail::RegisterToDrawer()
{
	for (auto& [id, t] : ribbonTrailCache_)
	{
		drawer_->Register(&t->GetTrail());
	}
	for (auto& [id, t] : shockwaveRingTrailCache_)
	{
		drawer_->Register(&t->GetTrail());
	}
}