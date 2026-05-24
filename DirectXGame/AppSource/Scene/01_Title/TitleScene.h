#pragma once
#include <Scene/IScene.h>
#include <UI/Title/TitleUI.h>
#include <Render/PostEffect.h>
#include <UI/Game/GameFrame.h>
#include <Camera/Camera.h>
#include <Camera/GameCamera.h>
#include <GameObject/Map/Map.h>
#include <GameObject/Player/Player.h>
#include <GameObject/Player/Controller/IController.h>
#include <GameObject/Player/Controller/InputController.h>
#include <memory>
#include "GameObject/Fade/FadeManager.h"
#include <02_ShigeScene/ShopScene.h>
#include <Render/RenderObject.h>
#include <Tool/Grid/Grid.h>

// ダミーコントローラー（タイトル画面用）
class DummyController : public IController {
public:
	Vector2 GetMoveDirection(float deltaTime, const MapInfo& mapInfo) override { 
		return {0.0f, 0.0f}; 
	}
	bool IsDashTriggered() override { return false; }
	void DrawImGui() override {}
};

class TitleScene : public IScene {
public:

	TitleScene();
	~TitleScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<TitleUI> titleUI_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<GameCamera> gameCamera_;
	std::unique_ptr<Map> map_;
	std::unique_ptr<Player::Base> player_;
	std::unique_ptr<DummyController> dummyController_;
	std::unique_ptr<InputController> inputController_;

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

	DirectionalLight dirLight_;

	// ターゲットマーカー関連
	std::unique_ptr<SHEngine::RenderObject> targetMarkerRender_;
	Transform targetMarkerTransform_;
	int targetMarkerTexIndex_ = 0;
	bool isTargetMarkerVisible_ = false;
	float targetMarkerAnimTimer_ = 0.0f;

	// yuka.obj描画関連
	std::unique_ptr<SHEngine::RenderObject> yukaRender_;
	Transform yukaTransform_;
	int yukaTexIndex_ = 0;

	//@brief オプションで使用する音量の変数(min 0.0, max 1.0)
	// CommonDataから読み込み、変更をCommonDataに反映する
	float* masterVolume_ = nullptr;
	float* bgmVolume_ = nullptr;
	float* seVolume_ = nullptr;

	//@brief 計算済みの音量（再生時に使用）
	float calculatedBgmVolume_ = 0.5f;
	float calculatedSeVolume_ = 0.5f;

	//@brief オプション選択中かどうか
	bool isOptionMode_ = false;

	//@brief 音量を再計算する
	void UpdateCalculatedVolumes();

	Option::Select currentOptionSelect_ = Option::Select::Master;
	std::unique_ptr<PlayData> bgm_ = nullptr;

	std::unique_ptr<FadeManager> fadeManager_;

	// Frame1滞在タイマー
	float frame1StayTimer_ = 0.0f;
	static constexpr float kFrame1StayDuration_ = 3.0f;

	// Frame3滞在タイマー
	float frame3StayTimer_ = 0.0f;

	MapInfo mapInfo_;
private:// Shop

	std::unique_ptr<ShopScene> shopScene_;

	// グリッド
	std::unique_ptr<Grid> grid_;

	std::unique_ptr<GameFrame> gameFrame_;
};