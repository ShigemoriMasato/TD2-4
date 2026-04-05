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
		int maxTrails = 512;
		int maxSegmentsPerTrail = 32; // Trail::Config::maxSegments の最大想定
		bool drawNormal = true;
		bool drawAdd = true;
	};

public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, const Config& cfg = {});
	void SetConfig(const Config& cfg);

	void Clear();
	void Register(Trail* trail);

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
	void RebuildIndexBuffer_();
	void BuildVertices_();

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	Config config_{};

	std::vector<Trail*> trails_;

	int maxVertexCountPerTrail_ = 0;
	int maxVertexCountTotal_ = 0;

	std::vector<BatchVertex> batchVertices_;

	// draw data
	int drawDataIndex_ = -1;

	// renderers（Normal/Add）
	std::unique_ptr<SHEngine::RenderObject> renderNormal_;
	std::unique_ptr<SHEngine::RenderObject> renderAdd_;

	// buffer indices
	int srvVertexIndex_ = -1; // VS t0
	int cbvVpIndex_ = -1;     // VS b0
	int cbvColorIndex_ = -1;  // PS b0
};