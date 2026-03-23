#pragma once
#include <GameObject/Effect/Trail/Trail.h>

class TestTrail3
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(float deltaTime, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Trigger(const Vector3& centerWS);

private:
	Trail trail_;

	int maxSegments_ = 32;

	float radius_ = 6.0f; // 衝撃波の半径

	// 発生状態
	bool active_ = false;
	float time_ = 0.0f;

	// 発生位置
	Vector3 centerWS_ = { 0.0f, 0.0f, 0.0f };
};