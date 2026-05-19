#pragma once
#include <GameObject/Effect/Trail/ITrail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>

class RibbonTrail : public ITrail
{
public:
	void SetConfig(const TrailPresetVariant& config) override;
	void Update(float dt) override;

private:
	// samples_の情報からuvや色を計算してgpuVertices_にセットする
	void RebuildVertices();

	struct Sample
	{
		Vector3 base;
		Vector3 tip;
		float age = 0.0f; // 秒
		float u = 0.0f;   // 0..1（長さ方向）
	};

	std::deque<Sample> samples_;
};