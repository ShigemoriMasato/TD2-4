#include "RibbonTrail.h"
#include <algorithm>

namespace
{
	// 2点間の距離
	float DistanceVec3Sq(const Vector3& a, const Vector3& b)
	{
		const float dx = a.x - b.x;
		const float dy = a.y - b.y;
		const float dz = a.z - b.z;
		return dx * dx + dy * dy + dz * dz;
	}
	float DistanceVec3(const Vector3& a, const Vector3& b)
	{
		return sqrtf(DistanceVec3Sq(a, b));
	}
}

void RibbonTrail::SetConfig(const TrailPresetVariant& config)
{
	config_ = config;
	const RibbonTrailConfig& uniqueConfig = std::get<RibbonTrailConfig>(config_);
	SetTexture(uniqueConfig.cfg.texturePath);

	// (i0, i1, i2).(i2, i1, i3) の順で入れるGPUに渡す方。
	gpuVertices_.clear();
	gpuVertices_.reserve((uniqueConfig.cfg.maxSegments + 1) * 2);
}

void RibbonTrail::Update(float dt)
{
	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && samples_.empty()) return;

	Clear();

	const RibbonTrailConfig& uniqueConfig = std::get<RibbonTrailConfig>(config_);


	if (isActive_)
	{
		// 現在のbase/tip位置を計算
		const Vector3 base = uniqueConfig.originLocal * modelWorld_;
		const Vector3 tip = uniqueConfig.tipLocal * modelWorld_;

		// 1フレーム前のbase/tip位置を取得
		const Vector3 preBase = samples_.empty() ? base : samples_.back().base;
		const Vector3 preTip = samples_.empty() ? tip : samples_.back().tip;

		// 面以上である
		if (!samples_.empty())
		{
			const float db = DistanceVec3Sq(base, preBase);
			const float dt = DistanceVec3Sq(tip, preTip);
			// 最小距離以上動いてないならreturn
			if (std::max(db, dt) < uniqueConfig.cfg.minDistance * uniqueConfig.cfg.minDistance)
			{
				return;
			}
		}

		// 新しいサンプルを追加
		Sample s;
		s.base = base;
		s.tip = tip;
		s.age = 0.0f;
		samples_.push_back(s);
	}

	// 年齢更新
	for (auto& s : samples_)
	{
		s.age += dt;
	}

	// 寿命切れ削除
	samples_.erase(
		std::remove_if(samples_.begin(), samples_.end(),
			[&uniqueConfig](const Sample& s)
			{
				return s.age >= uniqueConfig.cfg.lifeTime;
			}),
		samples_.end());

	// 最大サンプル数 = maxSegments + 1
	const int maxSamples = uniqueConfig.cfg.maxSegments + 1;
	while (int(samples_.size()) > maxSamples)
	{
		// オーバーしてたら古いものから削除
		samples_.pop_front();
	}
}


void RibbonTrail::RebuildVertices()
{
	// サンプルが2未満(面未満)なら何もしない
	if (samples_.size() < 2)
	{
		//activeVertexCount_ = 0;
		return;
	}

	const RibbonTrailConfig& uniqueConfig = std::get<RibbonTrailConfig>(config_);


	// 順番に頂点を入れるための配列。サンプル数*2の頂点が必要。
	const size_t stripCount = samples_.size() * 2;
	std::vector<GpuVertex> vertices(stripCount);
	gpuVertices_.clear();

	// uv計算 
	// サンプル数最大の時に後続サンプルが1になるように0..1に割り当てる。
	// 例：maxSegments=4のとき、サンプル数が5なら u=0,0.25,0.5,0.75,1 になるようにする。
	// 例：maxSegments=4のとき、サンプル数が3なら u=0,0.25,0.5 になるようにする。
	// サンプル数がすくない時にテクスチャがｷﾞｭってならないようにするための工夫。
	// 
	// 色計算
	// 年齢でフェードさせる。古いほど透明になる。
	// 例：lifeTime=1.0fのとき、age=0.50fならアルファは0.50fになる。
	// 例：lifeTime=1.0fのとき、age=0.75fならアルファは0.25fになる。

	const int maxSamples = uniqueConfig.cfg.maxSegments + 1; // 最大サンプル数
	for (size_t i = 0; i < samples_.size(); ++i)
	{
		// サンプル[i]取得
		const auto& s = samples_[i];

		// u計算
		float u = float(i) / (maxSamples - 1);
		vertices[i * 2 + 0].uv = { u, 0.0f };
		vertices[i * 2 + 1].uv = { u, 1.0f };

		// 色計算
		const float t = std::clamp(1.0f - (s.age / uniqueConfig.cfg.lifeTime), 0.0f, 1.0f);
		const Vector4 color = Vector4(uniqueConfig.cfg.color.x, uniqueConfig.cfg.color.y, uniqueConfig.cfg.color.z, uniqueConfig.cfg.color.w * t);
		vertices[i * 2 + 0].color = color;
		vertices[i * 2 + 1].color = color;

		// 座標設定
		vertices[i * 2 + 0].position = Vector4(s.base, 1.0f);
		vertices[i * 2 + 1].position = Vector4(s.tip, 1.0f);

		// テクスチャ設定
		vertices[i * 2 + 0].textureIndex = static_cast<uint32_t>(textureHandle_);
	}

	size_t dst = 0;
	for (size_t i = 0; i < samples_.size() - 1; ++i)
	{
		// strip の 2 頂点ずつ
		const GpuVertex& i0 = vertices[i * 2 + 0];
		const GpuVertex& i1 = vertices[i * 2 + 1];
		const GpuVertex& i2 = vertices[i * 2 + 2];
		const GpuVertex& i3 = vertices[i * 2 + 3];

		// 三角形1 (i0, i1, i2)
		verticesForGPU[dst++] = i0;
		verticesForGPU[dst++] = i1;
		verticesForGPU[dst++] = i2;

		// 三角形2 (i2, i1, i3)
		verticesForGPU[dst++] = i2;
		verticesForGPU[dst++] = i1;
		verticesForGPU[dst++] = i3;
	}



}