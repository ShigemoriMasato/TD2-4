#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Common/DebugParam/ParamManager.h>
#include <Render/PostEffect.h>
#include <Collision/ColliderManager.h>

#include<vector>
#include<array>

#include"Unit/Player/PlayerUnitObject.h"
#include"UI/SelectStageUI.h"
#include"GameCommon/DefaultObject.h"

class SelectScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private://エンジン的に必要な奴ら

	DualDisplay* display_ = nullptr;
	GameWindow* gameWindow_ = nullptr;

private:

	std::unique_ptr<ParamManager> paramManager_ = nullptr;

	std::unique_ptr<PostEffect> postEffect_ = nullptr;
	PostEffectConfig postEffectConfig_{};
	Blur blur_{};

	//=================================
	// カメラ
	//=================================

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	Camera camera_;

	// ui描画用のカメラ
	std::unique_ptr<Camera> uiCamera_;

	//==============================================
	// プレイヤー
	//==============================================

	// プレイヤーのオブジェクトデータ
	std::unique_ptr<PlayerUnitObject> playerObject_;

	bool isPlayerAnimation_ = false;

	Vector3 startPos_ = {};
	Vector3 endPos_ = {};

	float timer_ = 0.0f;
	float moveTime_ = 1.0f;

	//====================================================
	// ステージ
	//====================================================

	std::array<std::unique_ptr<DefaultObject>,3> stagePointObjects_;

	//==============================================
	// UI
	//==============================================

	// 選択しているステージ番号
	int selectStageNum_ = 1;
	int maxStageNum_ = 3;

	// ステージ選択のロゴ
	std::unique_ptr<SelectStageUI> selectStageUI_;
};
