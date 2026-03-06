#pragma once
#include <Render/RenderObject.h>
#include <vector>
#include <GameObject/BackPack/GameConstants.h>
#include "GameObject/BackPack/Structures.h"

class PrimitiveLineDrawer
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, uint32_t maxLines = 1024);

	// そのフレームに描画したい線分を積む
	void Clear();
	void AddLine(const Vector3& start, const Vector3& end, uint32_t color = 0xffffffff);
	void AddAABB(const AABB& aabb, uint32_t color = 0xff00ffff);
	void AddPlaneXZ(const PlaneXZ& plane, uint32_t color = 0xff00ff00);

	// VP更新＆GPU転送（Add系を呼んだ後に1回）
	void Update(const Matrix4x4& vpMatrix);

	void Draw(CmdObj* cmdObj);

private:
	struct LineConfig
	{
		Vector3 start;
		float padding;
		Vector3 end;
		uint32_t color;
	};

	void EnsureCapacity(uint32_t requiredLines);

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	std::vector<LineConfig> cpuLines_;

	uint32_t maxLines_ = 0;
	int drawDataIndex_ = -1;
};