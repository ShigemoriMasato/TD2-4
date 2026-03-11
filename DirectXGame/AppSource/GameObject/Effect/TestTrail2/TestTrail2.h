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

	Vector3 swordCenterWS_ = { 0.0f, 2.0f, 0.0f };
	Vector3 swordUpWS_ = { 0.0f, 1.0f, 0.0f };


	float auraRadius_ = 0.75f;     // 剣の周りの半径
	float auraWidth_ = 3.0f;      // Trailのbase-tip間の“太さ”
	float auraLength_ = 10.0f;      // Trailの長さ（剣に沿って伸ばす長さ）
	float spinSpeed_ = 8.0f;       // 回転速度
	float waveSpeed_ = 4.0f;       // 揺れ速度
	float waveAmp_ = 0.18f;        // 揺れ幅（半径に加算）
};