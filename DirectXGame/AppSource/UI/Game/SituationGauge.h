#pragma once
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>
#include <UI/Game/SituationTelop.h>

/// <summary>
/// 戦況状況を視覚化するためのクラス
/// </summary>
class SituationGauge {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime, float enemySpawnCount, float weaponCount, std::unordered_map<Key, bool> key);
	void Draw(CmdObj* cmdObj);

private:
	enum class Advantage{
		Even,
		Player,
		Enemy,
	};

private:
	// 現在の有利不利を受け取る
	Advantage GetAdvantage(float playerRatio, float enemyRatio) const;

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	int gaugeCount_ = 2;
	int textureIndex_ = 0;

	Vector4 playerColor_ = {0.0, 1.0f, 1.0f, 1.0f};
	Vector4 enemyColor_ = {1.0, 0.0f, 0.0f, 1.0f};

	// 戦力計算用の重み
	float weaponPowerWeight_ = 2.0f; // 武器1つあたりの戦力値（敵4体分）
	float enemyPowerWeight_ = 1.0f;  // 敵1体あたりの戦力値

	float currentIntensity_ = 0.5f; // 現在の戦況
	float lerpSpeed_ = 2.0f;        // 補間の速さ

	// バー同士の隙間
	float barSpacing_ = 40.0f;

	// 割合を計算するための最大値
	float maxWeaponCount_ = 10.0f; // 最大武器数
	float maxEnemyCount_ = 50.0f;  // 最大敵数

	// 現在の割合
	float currentPlayerRatio_ = 0.0f;
	float currentEnemyRatio_ = 0.0f;

	// 戦況テロップ
	std::unique_ptr<SituationTelop> situationTelop_;

	// 戦況変化の検出とテロップ表示のためのタイマー
	float timeSinceLastChange_ = 0.0f;

	Advantage wasAdvantage_ = Advantage::Even;
	Advantage currentAdvantage_ = Advantage::Even;

	// ゲージの最小値
	float minGaugeHeight_ = 10.0f;

	float time_ = 0.0f;
};