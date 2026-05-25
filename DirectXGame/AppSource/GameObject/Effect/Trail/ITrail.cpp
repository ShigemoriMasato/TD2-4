#include "ITrail.h"
#include <algorithm>
#include <cmath>

using namespace SHEngine;

void ITrail::Initialize(SHEngine::TextureManager* textureManager)
{
	textureManager_ = textureManager;

	gpuVertices_.resize(kMaxVertices_);
	Clear();
}

void ITrail::Clear()
{
	// GPUに送るようの頂点情報を初期化
	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});

	// アクティブ頂点数リセット
	activeVertexCount_ = 0;
}


//void Trail::SetConfig(const Config& config)
//{
//	config_ = config;
//	// 分割数を1以上に
//	config_.maxSegments = std::max(1, config_.maxSegments);
//	// 寿命を0.001秒以上に
//	config_.lifeTime = std::max(0.001f, config_.lifeTime);
//	// 最小距離を0以上に
//	config_.minDistance = std::max(0.0f, config_.minDistance);
//	// (分割数 + 1) * 2点分の頂点を用意
//	maxVertexCount_ = (config_.maxSegments + 1) * 2;
//	// リサイズ
//	gpuVertices_.resize(maxVertexCount_);
//	// 初期化
//	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});
//	// テクスチャセット
//	SetTexture(config_.texturePath);
//}

//void Trail::Clear()
//{
//	// 履歴クリア
//	samples_.clear();
//	hasLast_ = false;
//
//	// GPUに送るようの頂点情報を初期化
//	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});
//
//	// アクティブ頂点数リセット
//	activeVertexCount_ = 0;
//}


//void Trail::Update(float deltaTime)
//{
//	// 無効なら何もしない
//	if (!emitting_) return;
//
//	// age更新 + 寿命を超えたものを削除
//	for (auto& s : samples_)
//	{
//		s.age += deltaTime;
//	}
//	while (!samples_.empty() && samples_.front().age >= config_.lifeTime)
//	{
//		samples_.pop_front();
//	}
//
//	// 頂点再構築
//	RebuildVertices();
//}
