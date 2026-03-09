#pragma once
#include <GameObject/Effect/Trail.h>

class TestTrail2
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

	// 後で剣に追従させるための入口（今はテスト用に任意設定できるように）
	void SetSwordCenterWS(const Vector3& centerWS) { swordCenterWS_ = centerWS; }
	void SetSwordUpWS(const Vector3& upWS) { swordUpWS_ = upWS; }

private:
	Trail trail_;
	float time_ = 0.0f;

	// テスト用：剣の中心と軸（ワールド）
	Vector3 swordCenterWS_ = { 0.0f, 2.0f, 0.0f };
	Vector3 swordUpWS_ = { 0.0f, 1.0f, 0.0f };
};