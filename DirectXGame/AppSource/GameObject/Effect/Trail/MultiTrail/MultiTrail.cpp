#include "MultiTrail.h"
#include <stdexcept>
#include <Scene/CommonData.h>
#include <GameObject/Effect/Trail/ITrail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>
#include <GameObject/Effect/Trail/Drawer/TrailDrawer.h>

#include <GameObject/Effect/Trail/Type/RibbonTrail/RibbonTrail.h>
#include <GameObject/Effect/Trail/Type/ShockwaveRingTrail/ShockwaveRingTrail.h>

int32_t MultiTrail::Add(const std::string& presetName)
{
	if (!presetData_) return -1;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	nextId_++;

	std::unique_ptr<ITrail> trail;

	// トレイルタイプごとに生成
	if (std::holds_alternative<RibbonTrailConfig>(presetVar))
	{
		const auto& preset = std::get<RibbonTrailConfig>(presetVar);
		trail = std::make_unique<RibbonTrail>();
		trail->Initialize(textureManager_);
		trail->SetConfig(preset);
	}
	else if (std::holds_alternative<ShockwaveRingConfig>(presetVar))
	{
		const auto& preset = std::get<ShockwaveRingConfig>(presetVar);
		trail = std::make_unique<ShockwaveRingTrail>();
		trail->Initialize(textureManager_);
		trail->SetConfig(preset);
	}
	else
	{
		throw std::runtime_error("Invalid trail preset type");
	}

	trailCache_[nextId_] = std::move(trail);

	return nextId_;
}

void MultiTrail::SetModelWorld(const Matrix4x4& modelWorld)
{
	for (auto& [id, trail] : trailCache_)
	{
		trail->SetModelWorld(modelWorld);
	}
}

void MultiTrail::SetEmittingFlag(const int32_t id, bool flag)
{
	if (trailCache_.find(id) != trailCache_.end())
	{
		trailCache_[id]->SetIsActive(flag);
	}
}

void MultiTrail::SetConfig(const int32_t id, const TrailPresetVariant& presetVar)
{
	if (trailCache_.find(id) != trailCache_.end())
	{
		trailCache_[id]->SetConfig(presetVar);
	}	
}

TrailPresetVariant MultiTrail::GetConfig(const int32_t id)
{
	if (trailCache_.find(id) != trailCache_.end())
	{
		return trailCache_[id]->GetUniqueConfig();
	}
	return {};
}

void MultiTrail::Clear()
{
	for (auto& [id, trail] : trailCache_)
	{
		trail->Clear();
	}
}


void MultiTrail::Initialize(SHEngine::TextureManager* textureManager, CommonData* commonData)
{
	textureManager_ = textureManager;
	presetData_ = &commonData->trailPresetDataBank;
	drawer_ = commonData->trailDrawer.get();

	nextId_ = -1;

	trailCache_.clear();
}


void MultiTrail::Update(float dt)
{
	for (auto& [id, trail] : trailCache_)
	{
		trail->Update(dt);
	}
}

void MultiTrail::Draw()
{
	RegisterToDrawer();
}

void MultiTrail::RegisterToDrawer()
{
	for (auto& [id, trail] : trailCache_)
	{
		drawer_->Register(trail.get());
	}
}