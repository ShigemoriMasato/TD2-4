#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <array>
#include <assets/Model/ModelManager.h>

class ParameterRender {
public:
	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine);

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update(Matrix4x4 vpMatrix);

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw(CmdObj* cmdObj);

private:
	// パラメータの数
	static const int kParameterCount = 13;

	// 描画用変数
	std::array<std::unique_ptr<SHEngine::RenderObject>, kParameterCount> renders_ = {};

	// WVP行列
	std::array<Matrix4x4, kParameterCount> wvps_ = {};

	// トランスフォーム
	std::array<Transform, kParameterCount> transforms_ = {};

	// テクスチャインデックス
	std::array<int, kParameterCount> textureIndexes_;

	// テクスチャパス
	const std::array<std::string, kParameterCount> texturePaths = {"MaxHP.png",        "HpRegen.png",   "LifeSteal.png", "Damage.png",    "MeleeDamage.png", "rangedDamage.png", "AttackSpeed.png",
	                                                               "criticalRate.png", "dodgeRate.png", "defense.png",   "moveSpeed.png", "expBonus.png",    "moneyBonus.png"};

	float startPosY_ = 0.0f;
	float posZ_ = 10.0f;

	// 2D用正射影カメラのVP行列を保存
	Matrix4x4 orthoVPMatrix_ = Matrix4x4::Identity();
};
