#pragma once
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include <GameObject/Player/Parameter/ParameterData.h>
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <array>
#include <assets/Model/ModelManager.h>
#include <Common/KeyConfig/KeyManager.h>

class ParameterRender {
public:
	/// <summary>
	/// 初期化関数
	/// </summary>
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine);

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update(Matrix4x4 vpMatrix, const std::unordered_map<std::string, float>& parameterData, float deltaTime, std::unordered_map<Key, bool> key);

	/// <summary>
	/// 描画関数
	/// </summary>
	void Draw(CmdObj* cmdObj);

private:
	/// <summary>
	/// アニメーションの開始関数
	/// </summary>
	void AnimationStart();

	/// <summary>
	/// 戻りアニメーションの開始関数
	/// </summary>
	void ReturnAnimationStart();

private:
	// パラメータの数
	static const int kParameterCount = 14;

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
	const std::array<std::wstring, kParameterCount> texturePaths = {L"MaxHP",       L"HPRegen",   L"Damage",    L"MeleeDamage", L"RangedDamage", L"AttackSpeed", L"AttackCount",
	                                                                L"Penetration", L"KnockBack", L"LifeSteal", L"Move",        L"Defence",      L"EXP",         L"Gold"};

	// パラメータ表示
	float startPosY_ = -100.0f;   // 開始位置
	float posX_ = 1240.0f;         // X座標
	float marginY_ = -40.0f;      // 間隔
	float valueOffsetX_ = 270.0f; // ラベルから数値までのXオフセット

	// テキスト（ラベル用と数値用）
	std::array<std::unique_ptr<SHEngine::Text>, kParameterCount> texts_;
	std::array<std::unique_ptr<SHEngine::Text>, kParameterCount> valueTexts_;
	std::array<Transform, kParameterCount> valueTransforms_ = {};

	// アニメーション用の変数
	AnimationBundle<float> offsetAnimation_;
	bool isAnimation_ = false;
	float endPos_ = -300.0f;
};