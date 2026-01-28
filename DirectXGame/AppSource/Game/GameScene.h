#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Common/DebugParam/ParamManager.h>
#include <Render/PostEffect.h>
#include <Collision/ColliderManager.h>

#include"Unit/UnitManager.h"
#include"Unit/MapChipField.h"
#include"Unit/MapChipRender.h"
#include"Item/OreItemManager.h"
#include"GameCamera/CameraController.h"
#include"UI/FadeTransition.h"
#include"UI/Object/FontObject.h"
#include"UI/CounterUI.h"
#include"UI/TimerUI.h"
#include"TimeTracker.h"

#include <ModelEditScene/Render/MapRender.h>
#include <ModelEditScene/Render/DebugMCRender.h>

// ゲームオーバーシーン
#include"UI/GameOverUI.h"
// クリアシーン
#include"UI/ClearUI.h"

class GameScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private://エンジン的に必要な奴ら

	DualDisplay* display_ = nullptr;
	GameWindow* gameWindow_ = nullptr;

private:

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	Camera camera_;

	// ui描画用のカメラ
	std::unique_ptr<Camera> uiCamera_;

	// 当たり判定管理クラス
	std::unique_ptr<ColliderManager> colliderManager_;

	int param1;
	float param2;
	Vector3 param3;
	Vector4 param4;
	std::string param5;

	std::unique_ptr<ParamManager> paramManager_ = nullptr;

	std::unique_ptr<PostEffect> postEffect_ = nullptr;
	PostEffectConfig postEffectConfig_{};
	Blur blur_{};
	
	// シーン切り替え
	bool isSceneChange_ = false;

	// 遷移の処理
	std::unique_ptr<FadeTransition> fadeTransition_;

	//=====================================================
	// インゲーム
	//=====================================================

	// ゲームカメラの管理
	std::unique_ptr<CameraController> cameraController_;

	// マップデータの中間管理職
	std::unique_ptr<MapChipField> mapChipField_;

	// マップの描画
	std::unique_ptr<MapRender> mapRender_;
	std::unique_ptr<DebugMCRender> debugMapRender_;//マップチップを単純な色で描画
	std::map<TileType, Vector3> colorMap_;//DebugMCRenderに使う、マップチップの色

	// 現在のマップ
	NewMap currentMap_;

	// 鉱石の管理
	std::unique_ptr<OreItemManager> oreItemManager_;

	// ユニット管理
	std::unique_ptr<UnitManager> unitManager_;

	// ユニットの数UI
	std::unique_ptr<CounterUI> unitCounterUI_;
	// 鉱石の数UI
	std::unique_ptr<CounterUI> oreItemUI_;

	// 時間を測る
	std::unique_ptr<TimeTracker> timeTracker_;

	// 計測時間をだすUI
	std::unique_ptr<TimerUI> timerUI_;

	//================================================
	// ゲームオーバーシーン
	//================================================

	// ゲームオーバーUI
	std::unique_ptr<GameOverUI> gameOverUI_;

	// ゲームオーバーシーン
	bool isGameOverScene_ = false;

	//================================================
	// クリアシーン
	//================================================

	// クリアUI
	std::unique_ptr<ClearUI> clearUI_;

	// クリアシーンを判断
	bool isClearScene_ = false;

	//================================================
	// Tool系
	//================================================
	//Debug用ColorMapのためのやつ
	BinaryManager binaryManager_;

private:

	// ゲームの実行処理
	void InGameScene();

	// 他のシーンの初期化処理
	void InitializeOtherScene();

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();

	/// <summary>
	/// ColorMapを読み込む
	/// </summary>
	void LoadDebugColorMap();

	/// <summary>
	/// 鉱床の配置
	/// </summary>
	void PutGold();
};
