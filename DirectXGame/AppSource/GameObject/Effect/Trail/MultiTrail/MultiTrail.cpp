#include "MultiTrail.h"
#include <stdexcept>

void MultiTrail::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	TrailPresetRepository* presetRepo)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	presetRepo_ = presetRepo;

	if (!drawDataManager_ || !textureManager_ || !presetRepo_)
	{
		throw std::runtime_error("MultiTrail::Initialize: null dependency");
	}

	entries_.clear();
	enabled_ = true;
	modelWorld_ = Matrix4x4::Identity();
}

void MultiTrail::AddFromPresetName(const std::string& presetName)
{
	if (!presetRepo_) return;

	const auto& presetVar = presetRepo_->Get(presetName);

	Entry e{};
	e.presetName = presetName;

	if (std::holds_alternative<RibbonTrailPreset>(presetVar))
	{
		auto& t = e.trail.emplace<RibbonTrail>();
		t.Initialize(drawDataManager_, textureManager_, std::get<RibbonTrailPreset>(presetVar));
	}
	else if (std::holds_alternative<ShockwaveRingPreset>(presetVar))
	{
		auto& t = e.trail.emplace<ShockwaveRingTrail>();
		t.Initialize(drawDataManager_, textureManager_, std::get<ShockwaveRingPreset>(presetVar));
	}
	else
	{
	}

	entries_.push_back(std::move(e));
}

void MultiTrail::TriggerShockwave(const Vector3& centerWS, const Vector3& normalWS)
{
	for (auto& e : entries_)
	{
		if (auto* s = std::get_if<ShockwaveRingTrail>(&e.trail))
		{
			s->Trigger(centerWS, normalWS);
		}
	}
}

void MultiTrail::Clear()
{
	entries_.clear();
}

void MultiTrail::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (!enabled_) return;

	for (auto& e : entries_)
	{
		std::visit(
			[&](auto& t)
			{
				using T = std::decay_t<decltype(t)>;
				if constexpr (std::is_same_v<T, RibbonTrail>)
				{
					t.SetModelWorld(modelWorld_);
					t.Update(dt, vpMatrix);
				}
				else if constexpr (std::is_same_v<T, ShockwaveRingTrail>)
				{
					t.Update(dt, vpMatrix);
				}
			},
			e.trail);
	}
}

void MultiTrail::Draw(CmdObj* cmdObj)
{
	if (!enabled_) return;

	for (auto& e : entries_)
	{
		std::visit(
			[&](auto& t)
			{
				t.Draw(cmdObj);
			},
			e.trail);
	}
}