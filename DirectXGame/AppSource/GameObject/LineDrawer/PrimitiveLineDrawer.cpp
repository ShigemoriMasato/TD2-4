#include "PrimitiveLineDrawer.h"
#include <cassert>

using namespace SHEngine;

void PrimitiveLineDrawer::Initialize(DrawDataManager* drawDataManager, uint32_t maxLines)
{
	assert(drawDataManager);

	maxLines_ = (maxLines == 0) ? 1u : maxLines;

	// Gridと同様：ダミー頂点2つ + デフォルトインデックス（0,1）が作られる
	if (drawDataIndex_ == -1)
	{
		std::vector<Vector3> vertices(2, {});
		drawDataManager->AddVertexBuffer(vertices);
		drawDataIndex_ = drawDataManager->CreateDrawData();
	}
	auto drawData = drawDataManager->GetDrawData(drawDataIndex_);

	render_ = std::make_unique<RenderObject>("PrimitiveLineDrawer");
	render_->Initialize();
	render_->SetDrawData(drawData);

	// Grid用シェーダを流用（SRV: t0 に LineBuffer、CBV: b0 に vp）
	render_->psoConfig_.vs = "Engine/Grid.VS.hlsl";
	render_->psoConfig_.ps = "Engine/Grid.PS.hlsl";
	render_->psoConfig_.inputLayoutID = PSO::InputLayoutID::Vector3;
	render_->psoConfig_.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	// SRV(線分配列) + CBV(VP)
	render_->CreateSRV(sizeof(LineConfig), maxLines_, ShaderType::VERTEX_SHADER, "LineConfig");
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");

	render_->instanceNum_ = 0;

	cpuLines_.reserve(maxLines_);
}

void PrimitiveLineDrawer::Clear()
{
	cpuLines_.clear();
}

void PrimitiveLineDrawer::EnsureCapacity(uint32_t requiredLines)
{
	// RenderObjectのSRVは作り直せない設計なので、上限超過は防ぐ（まずassert）
	// 必要なら「maxLinesを大きくしてInitializeし直す」運用にしてください。
	assert(requiredLines <= maxLines_ && "PrimitiveLineDrawer: exceeded maxLines. Increase maxLines in Initialize().");
}

void PrimitiveLineDrawer::AddLine(const Vector3& start, const Vector3& end, uint32_t color)
{
	EnsureCapacity(static_cast<uint32_t>(cpuLines_.size() + 1));

	cpuLines_.push_back({
		.start = start,
		.padding = 0.0f,
		.end = end,
		.color = color
		});
}

void PrimitiveLineDrawer::AddAABB(const AABB& aabb, uint32_t color)
{
	// 8 corners
	const Vector3 v0{ aabb.min.x, aabb.min.y, aabb.min.z };
	const Vector3 v1{ aabb.max.x, aabb.min.y, aabb.min.z };
	const Vector3 v2{ aabb.min.x, aabb.max.y, aabb.min.z };
	const Vector3 v3{ aabb.max.x, aabb.max.y, aabb.min.z };
	const Vector3 v4{ aabb.min.x, aabb.min.y, aabb.max.z };
	const Vector3 v5{ aabb.max.x, aabb.min.y, aabb.max.z };
	const Vector3 v6{ aabb.min.x, aabb.max.y, aabb.max.z };
	const Vector3 v7{ aabb.max.x, aabb.max.y, aabb.max.z };

	// 12 edges
	EnsureCapacity(static_cast<uint32_t>(cpuLines_.size() + 12));

	// bottom (min.z side)
	AddLine(v0, v1, color);
	AddLine(v1, v3, color);
	AddLine(v3, v2, color);
	AddLine(v2, v0, color);

	// top (max.z side)
	AddLine(v4, v5, color);
	AddLine(v5, v7, color);
	AddLine(v7, v6, color);
	AddLine(v6, v4, color);

	// vertical
	AddLine(v0, v4, color);
	AddLine(v1, v5, color);
	AddLine(v2, v6, color);
	AddLine(v3, v7, color);
}

void PrimitiveLineDrawer::AddPlaneXZ(const PlaneXZ& plane, uint32_t color)
{
	const float hx = plane.width * 0.5f;
	const float hz = plane.height * 0.5f;

	const Vector3 p0{ plane.center.x - hx, plane.center.y, plane.center.z - hz };
	const Vector3 p1{ plane.center.x + hx, plane.center.y, plane.center.z - hz };
	const Vector3 p2{ plane.center.x + hx, plane.center.y, plane.center.z + hz };
	const Vector3 p3{ plane.center.x - hx, plane.center.y, plane.center.z + hz };

	EnsureCapacity(static_cast<uint32_t>(cpuLines_.size() + 4));

	AddLine(p0, p1, color);
	AddLine(p1, p2, color);
	AddLine(p2, p3, color);
	AddLine(p3, p0, color);
}

void PrimitiveLineDrawer::Update(const Matrix4x4& vpMatrix)
{
	if (!render_) return;

	// SRVへ転送
	if (!cpuLines_.empty())
	{
		render_->CopyBufferData(0, cpuLines_.data(), sizeof(LineConfig) * cpuLines_.size());
	}

	// VP更新
	render_->CopyBufferData(1, &vpMatrix, sizeof(Matrix4x4));

	// インスタンス数=線分本数
	render_->instanceNum_ = static_cast<uint32_t>(cpuLines_.size());
}

void PrimitiveLineDrawer::Draw(CmdObj* cmdObj)
{
	if (!render_) return;
	render_->Draw(cmdObj);
}