#pragma once
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// 戦況状況を視覚化するためのクラス
/// </summary>
class SituationGauge {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime, float enemySpawnCount, float weaponCount);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	int kGaugeCount = 2;
	int textureIndex = 0;

	Vector4 playerColor_ = {0.0, 1.0f, 1.0f, 1.0f};
	Vector4 enemyColor_ = {1.0, 0.0f, 0.0f, 1.0f};

	// 戦力計算用の重み
	float weaponPowerWeight_ = 4.0f; // 武器1つあたりの戦力値（敵4体分）
	float enemyPowerWeight_ = 1.0f;  // 敵1体あたりの戦力値

	float currentIntensity_ = 0.5f; // 現在の戦況
	float lerpSpeed_ = 2.0f; // 補間の速さ
};
