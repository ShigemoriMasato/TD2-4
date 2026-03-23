#include "TestTrail1.h"

void TestTrail1::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager)
{
	// トレイル初期化
	Trail::Config cfg{};
	cfg.maxSegments = 32;
	cfg.lifeTime = 0.25f;
	cfg.minDistance = 0.0f;
	cfg.drawNormal = true;
	cfg.drawAdd = false;
	cfg.texturePath = "Assets/.EngineResource/Texture/uvChecker.png";
	trail_.Initialize(drawDataManager, textureManager, cfg);
}

void TestTrail1::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	time_ += deltaTime;

	const float r = 6.0f;
	const float w = 4.0f; // 角速度
	const float s = std::sin(time_ * w);
	const float c = std::cos(time_ * w);

	// base と tip（ワールド座標）
	Vector3 baseWS = { c * r, 2.0f, s * r };
	Vector3 tipWS = { c * r, 2.0f + 5.0f, s * r };

	trail_.PushSegment(baseWS, tipWS);

	trail_.Update(deltaTime, vpMatrix);
}

void TestTrail1::Draw(CmdObj* cmdObj)
{
	trail_.Draw(cmdObj);
}