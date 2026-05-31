#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include <Compute/ComputeObject.h>
#include <GameObject/Player/Player.h>
#include <GameObject/Enemy/EnemyManager.h>
#include <GameObject/ObjectRender.h>
#include <Collision/ColliderManager.h>
#include <GameObject/Map/Map.h>
#include <GameObject/Weapon/WeaponDatabase.h>
#include <GameObject/Attack/AttackManager.h>
#include <GameObject/Weapon/IWeapon.h>
#include <GameObject/Weapon/WeaponInclude.h>
#include <Camera/GameCamera.h>
#include <GameObject/Weapon/Render/WeaponRenderInclude.h>
#include <GameObject/Item/ItemManager.h>
#include <GameObject/Enemy/LevelSystem.h>
#include <GameObject/GameTimer.h>
#include <GameObject/Player/Controller/AIController.h>
#include <GameObject/Player/Controller/InputController.h>
#include <GameObject/Weapon/IWeaponRender.h>
#include <02_ShigeScene/ShopScene.h>
#include <Scene/01_Title/TitleScene.h>
#include <Scene/05_Result/ResultScene.h>
#include <GameObject/Player/PlayerHP.h>
#include <GameObject/Player/Parameter/ParameterRender.h>
#include <UI/Game/GameFrame.h>
#include <Render/PostEffect.h>
#include <Render/Font/Text.h>
#include <02_ShigeScene/GameDisplayRange.h>
#include <GameObject/Enemy/LevelSystemUI.h>
#include <GameObject/Enemy/TackleEnemy.h>
#include <UI/Game/SituationTelop.h>
#include <GameObject/Effect/Flash/Flash.h>
#include <GameObject/Effect/LetterBox/LetterBox.h>
#include <GameObject/Enemy/EnemyEffect/EnemyEffectManager.h>
#include "GameObject/Fade/FadeManager.h"
#include "UI/Game/Pause/PauseMenu.h"
#include "UI/Game/WeaponList/WeaponList.h"

class ShigeScene : public IScene {
public:

	~ShigeScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void MakeWeapon();

private:// System系
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<GameCamera> gameCamera_;
	std::unique_ptr<GameCamera> pauseCamera_;
	Camera* camera_;

	std::unique_ptr<Grid> grid_;
	std::unique_ptr<ColliderManager> colliderManager_;

	std::unique_ptr<Player::Base> player_;
	std::unique_ptr<Player::HP> playerHP_;
	std::unique_ptr<AIController> aiController_;
	std::unique_ptr<InputController> inputController_;

	std::unique_ptr<SHEngine::RenderObject> targetMarkerRender_;
	Transform targetMarkerTransform_;
	int targetMarkerTexIndex_ = 0;
	bool isTargetMarkerVisible_ = false;
	float targetMarkerAnimTimer_ = 0.0f;

	std::unique_ptr<SHEngine::RenderObject> moveingRender_;
	std::unique_ptr<SHEngine::RenderObject> autoMoveingRender_;
	int moveingTexIndex_ = 0;
	int autoMoveingTexIndex_ = 0;
	Transform moveingIndicatorOffset_ = {{1.0f, 1.0f, 1.0f}, {-1.11f, 0.0f, 0.0f}, {0.0f, 5.25f, 0.0f}};
	float moveingIndicatorAnimTimer_ = 0.0f;

	std::vector<std::unique_ptr<IWeapon>> weapons_;
	std::vector<std::unique_ptr<IWeaponRender>> weaponRenders_;
	std::vector<std::pair<int, std::unique_ptr<IWeaponRender>>> wrDeleting_;
	const int maxWeaponCount_ = 64;

	std::unique_ptr<Map> map_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<EnemyEffect> enemyEffectManager_;

	std::unique_ptr<WeaponDatabase> weaponDatabase_;
	std::unique_ptr<AttackManager> attackManager_;
	std::unique_ptr<LevelSystem> waveSystem_;
	std::unique_ptr<LevelSystemUI> waveSystemUI_;
	std::unique_ptr<GameTimer> gameTimer_;
	std::vector<IController*> controllers_;

	std::vector<DrawInfo> drawInfos_;
	std::unique_ptr<ObjectRender> objectRender_;
	std::unique_ptr<Camera> orthoCamera_;

	std::unique_ptr<Flash> flashEffect_;
	std::unique_ptr<LetterBox> letterBox_;
	std::unique_ptr<FadeManager> fadeManager_;

	JsonManager jsonManager_;

	float worldTimer_ = 0.0f;

	// 現在使用しているコントローラーのインデックス
	int currentControllerIndex_ = 0;

	// 武器の配置関連の変数
	float baseRadius_ = 4.0f; // 円の半径
	float baseHeight_ = 2.0f; // 高さ

	bool isCameraDragging_ = false;
	Vector3 cameraTargetOffset_ = {0.0f, 0.0f, 0.0f};
	Vector2 lastMousePos_ = {0.0f, 0.0f};

	//
	float bgmVolume_ = 0.5f;
	float seVolume_ = 0.5f;

	// ライティング
	DirectionalLight dirLight_;

	bool isPlayerDead_ = false;

	// カメラシェイク用
	bool isCameraShaking_ = false;
	float cameraShakeTime_ = 0.0f;
	float cameraShakeDuration_ = 0.3f;
	float cameraShakeIntensity_ = 0.5f;
	float prevPlayerHP_ = -1.0f;

	Vignette vignette_;
	AnimationBundle<float> vignetteIntensityAnim_;
	float vinetteIntensity_ = 0.2f;
	float vinetteActiveHP_ = 100.0f;

	// ポーズ
	bool isPause_ = false;
	std::unique_ptr<PauseMenu> pauseMenu_;

	// マウスカーソルスプライト
	std::unique_ptr<SHEngine::RenderObject> mouseCursorSprite_ = nullptr;
	int mouseCursorTextureIndex_ = -1;
	int mouseCursorTexDefault_ = -1;
	int mouseCursorTexLeft_ = -1;
	int mouseCursorTexRight_ = -1;
	int mouseCursorTexBoth_ = -1;
	Transform mouseCursorTransform_ = { {32.0f, 32.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	// mouseM スプライト
	std::unique_ptr<SHEngine::RenderObject> mouseMSprite_ = nullptr;
	int mouseMTexIndex_ = -1;
	Transform mouseMTransform_ = { {32.0f, 32.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {480.0f, -640.0f, 0.0f} };

	// ポーズテキスト
	std::unique_ptr<SHEngine::Text> pauseText_;
	Transform pauseTextTransform_ = { {0.8f, 0.8f, 1.0f}, {0.0f, 0.0f, 0.0f}, {500.0f, -648.0f, 0.0f} };
	Vector4 pauseTextColor_ = {1.0f, 1.0f, 1.0f, 1.0f};

private:// Shop

	std::unique_ptr<ShopScene> shopScene_;

private:// UI系

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

	std::unique_ptr<ShopDisplay> gameDisplay_;
	std::unique_ptr<GameFrame> gameFrame_;
	std::unique_ptr<GameFrame> gameFrameBG_;

	std::unique_ptr<SHEngine::Text> timerText_;
	Transform timerTextTransform_ = { {2.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {550.0f, -85.0f, 0.0f} };

	std::unique_ptr<SHEngine::Text> enemySpawnGraphText_;
	Transform enemySpawnGraphTextTransform_ = { {1.5f, 1.5f, 1.0f}, {0.0f, 0.0f, 0.0f}, {930.0f, -80.0f, 0.0f} };

	GameDisplayRange displayRange_ = {};

	std::unique_ptr<PlayData> bgm_ = nullptr;

	std::unique_ptr<WeaponList> weaponList_;
	bool isWeaponListVisible_ = false;
};
