#pragma once
#include <Render/RenderObject.h>
#include <Render/DrawDataManager.h>
#include <Utility/Matrix.h>
#include <vector>

class Trail;

class TrailDrawer final
{
public:
	struct Config
	{
		int maxTrails = 512;			// そのシーン内の最大トレイル数
		int maxSegmentsPerTrail = 32;	// 全トレイルの最大分割数がmaxSegmentsPerTrailになる
	};


public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, const Config& cfg = {});
	void SetConfig(const Config& cfg);

	void Clear();
	void Register(Trail* trail);

	// シーン内トレイルをすべて描画
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

private:
	struct BatchVertex
	{
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
		Vector4 color;
		uint32_t textureIndex;
	};

private:
	void BuildIndexBuffer();
	void BuildVertices();

private:
	// 外部ポインタ
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;

	// トレイル描画全体の設定
	Config config_{};

	// シーン内のトレイルリスト。
	std::vector<Trail*> trails_;

	// trails_に入っているすべてのトレイルの頂点をまとめる配列。サイズは config_.maxTrails * config_.maxSegmentsPerTrail * 2（base/tipの2点分）で固定。activeVertexCount_までが有効。
	std::vector<BatchVertex> batchVertices_;

	// 今フレーム描画する頂点数。trails_の内容によって毎フレーム変わる。最大で config_.maxTrails * config_.maxSegmentsPerTrail * 2。
	int maxVertexCountPerTrail_ = 0;
	// 上記の頂点数をすべてのトレイルで合計したもの。これも毎フレーム変わる。
	int maxVertexCountTotal_ = 0;

	// draw data
	int drawDataIndex_ = -1;

	// 描画オブジェクト
	std::unique_ptr<SHEngine::RenderObject> render_;
};