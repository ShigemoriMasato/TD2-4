#include "Trail.h"
#include <algorithm>
#include <cmath>

using namespace SHEngine;

namespace
{
	float DistanceVec3(const Vector3& a, const Vector3& b)
	{
		const float dx = a.x - b.x;
		const float dy = a.y - b.y;
		const float dz = a.z - b.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}
}

void Trail::Initialize(DrawDataManager* drawDataManager, TextureManager* textureManager, const Config& config)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	config_ = config;

	// 分割数を1以上に
	config_.maxSegments = std::max(1, config_.maxSegments);
	// 寿命を0.001秒以上に
	config_.lifeTime = std::max(0.001f, config_.lifeTime);
	// 最小距離を0以上に
	config_.minDistance = std::max(0.0f, config_.minDistance);

	// (分割数 + 1) * 2点分の頂点を用意
	maxVertexCount_ = (config_.maxSegments + 1) * 2;
	// リサイズ
	gpuVertices_.resize(maxVertexCount_);
	// 初期化
	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});

	// テクスチャ設定
	SetTexture(config_.texturePath);

	Clear();
}

void Trail::Clear()
{
	// 履歴クリア
	samples_.clear();
	hasLast_ = false;

	// GPUに送るようの頂点情報を初期化
	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});

	// アクティブ頂点数リセット
	activeVertexCount_ = 0;
}

void Trail::SetTexture(const std::string& texturePath)
{
	textureHandle_ = textureManager_->LoadTexture(texturePath);
}


void Trail::PushSegment(const Vector3& baseWS, const Vector3& tipWS)
{
	if (!enabled_) return;

	// 間引き（どちらかが一定以上動いたら追加）
	if (hasLast_)
	{
		const float db = DistanceVec3(baseWS, lastBase_);
		const float dt = DistanceVec3(tipWS, lastTip_);
		if (std::max(db, dt) < config_.minDistance)
		{
			return;
		}
	}

	// Base点記録
	lastBase_ = baseWS;
	// Tip点記録
	lastTip_ = tipWS;
	// 面以上になったフラグを立てる
	hasLast_ = true;

	// 新しいサンプルを追加
	Sample s;
	s.base = baseWS;
	s.tip = tipWS;
	s.age = 0.0f;
	samples_.push_back(s);

	// 最大サンプル数 = maxSegments + 1
	const int maxSamples = config_.maxSegments + 1;
	while (int(samples_.size()) > maxSamples)
	{
		// オーバーしてたら古いものから削除
		samples_.pop_front();
	}
}

void Trail::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	// 無効なら何もしない
	if (!enabled_) return;

	// age更新 + 寿命を超えたものを削除
	for (auto& s : samples_)
	{
		s.age += deltaTime;
	}
	while (!samples_.empty() && samples_.front().age >= config_.lifeTime)
	{
		samples_.pop_front();
	}

	// 頂点再構築
	RebuildVertices(vpMatrix);
}

void Trail::RebuildVertices(const Matrix4x4& vpMatrix)
{
	// サンプルが2未満(面未満)なら何もしない
	if (samples_.size() < 2)
	{
		activeVertexCount_ = 0;
		return;
	}

	// Uを0..1で割り当て
	const int n = int(samples_.size());
	for (int i = 0; i < n; ++i)
	{
		const float u = (n <= 1) ? 0.0f : (float(i) / float(n - 1));
		samples_[i].u = u;
	}

	// 頂点組み立て（base/tipの2頂点×サンプル数）
	const int vertexCount = std::min(n * 2, maxVertexCount_);
	activeVertexCount_ = vertexCount;
	for (int i = 0; i < vertexCount / 2; ++i)
	{
		const auto& s = samples_[i];
		// フェード（古いほど透明）
		const float t = std::clamp(1.0f - (s.age / config_.lifeTime), 0.0f, 1.0f);

		auto makeV = [&](const Vector3& p, float v) -> GpuVertex
			{
				GpuVertex out{};
				out.position = Vector4(p, 1.0f);
				out.uv = Vector2(s.u, v);
				out.normal = Vector3(0.0f, 1.0f, 0.0f); // ライトなし
				out.color = Vector4(1, 1, 1, t);
				return out;
			};

		gpuVertices_[i * 2 + 0] = makeV(s.base, 0.0f);
		gpuVertices_[i * 2 + 1] = makeV(s.tip, 1.0f);
	}

	// 残りはクリア（前フレームの残骸防止）
	for (int i = vertexCount; i < maxVertexCount_; ++i)
	{
		gpuVertices_[i] = GpuVertex{};
	}
}
