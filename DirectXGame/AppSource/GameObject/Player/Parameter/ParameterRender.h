#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <array>
#include <assets/Model/ModelManager.h>
#include <GameObject/Player/Parameter/ParameterData.h>

class ParameterRender {
public:
	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine);

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update(Matrix4x4 vpMatrix, const ParameterData& parameterData);

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw(CmdObj* cmdObj);

private:
	// パラメータの数
	static const int kParameterCount = 13;

	// 描画用変数
	std::array<std::unique_ptr<SHEngine::RenderObject>, kParameterCount> renders_ = {};

	// 背景描画用変数
	std::unique_ptr<SHEngine::RenderObject> backgroundRender_ = nullptr;

	// WVP行列
	std::array<Matrix4x4, kParameterCount> wvps_ = {};
	Matrix4x4 backgroundWVP_ = Matrix4x4::Identity();

	// トランスフォーム
	std::array<Transform, kParameterCount> transforms_ = {};
	Transform backgroundTransform_ = {};

	// テクスチャインデックス
	std::array<int, kParameterCount> textureIndexes_;

	// テクスチャパス
	const std::array<std::string, kParameterCount> texturePaths = {"MaxHP.png",        "HpRegen.png",   "LifeSteal.png", "Damage.png",    "MeleeDamage.png", "rangedDamage.png", "AttackSpeed.png",
	                                                               "criticalRate.png", "dodgeRate.png", "defense.png",   "moveSpeed.png", "expBonus.png",    "moneyBonus.png"};

	float startPosY_ = -30.0f; // 開始位置
	float margin_ = 55.0f;     // 間隔

	// 2D用正射影カメラのVP行列を保存
	Matrix4x4 orthoVPMatrix_ = Matrix4x4::Identity();

	// 0〜9の数字テクスチャインデックス
	std::array<int, 10> digitTextureIndexes_;

	// 各パラメータ×3桁分のRenderObjectとWVP・Transform
	std::array<std::array<std::unique_ptr<SHEngine::RenderObject>, 3>, kParameterCount> digitRenders_ = {};
	std::array<std::array<Transform, 3>, kParameterCount> digitTransforms_ = {};
	std::array<std::array<Matrix4x4, 3>, kParameterCount> digitWvps_ = {};

	// 現在の描画桁数（1〜3）
	std::array<int, kParameterCount> activeDigitCounts_ = {};

	// 表示する数字の配列
	std::array<std::array<int, 3>, kParameterCount> currentDigits_ = {};

	// 数字のレイアウト調整用
	float digitOffsetX_ = 110.0f;               // ラベルの基準位置から1桁目へのXオフセット
	float digitMarginX_ = 40.0f;                // 数字同士のX間隔
	Vector3 digitScale_ = {50.0f, 50.0f, 1.0f}; // 数字テクスチャのスケール
};
