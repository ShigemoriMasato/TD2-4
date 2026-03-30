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
#include <GameObject/Player/PlayerHP.h>
#include <GameObject/Player/Parameter/ParameterRender.h>
#include <UI/Game/GameFrame.h>
#include <Render/PostEffect.h>
#include <Render/Font/Text.h>
#include <02_ShigeScene/GameDisplayRange.h>
#include <GameObject/Enemy/LevelSystemUI.h>
#include <GameObject/Enemy/TackleEnemy.h>

class ShigeScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void MakeWeapon();

private:// System系
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<GameCamera> gameCamera_;
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

	std::vector<std::unique_ptr<IWeapon>> weapons_;
	std::vector<std::unique_ptr<IWeaponRender>> weaponRenders_;
	std::vector<std::pair<int, std::unique_ptr<IWeaponRender>>> wrDeleting_;
	const int maxWeaponCount_ = 64;

	std::unique_ptr<Map> map_;
	std::unique_ptr<EnemyManager> enemyManager_;

	std::unique_ptr<WeaponDatabase> weaponDatabase_;
	std::unique_ptr<AttackManager> attackManager_;
	std::unique_ptr<LevelSystem> waveSystem_;
	std::unique_ptr<LevelSystemUI> waveSystemUI_;
	std::unique_ptr<GameTimer> gameTimer_;
	std::vector<IController*> controllers_;

	std::vector<DrawInfo> drawInfos_;
	std::unique_ptr<ObjectRender> objectRender_;
	std::unique_ptr<Camera> orthoCamera_;

	std::unique_ptr<ParameterRender> parameterRender_;

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

private:// Shop

	std::unique_ptr<ShopScene> shopScene_;

private:// UI系

	std::unique_ptr<ShopDisplay> gameDisplay_;
	std::unique_ptr<GameFrame> gameFrame_;
	std::unique_ptr<GameFrame> gameFrameBG_;

	std::unique_ptr<SHEngine::Text> timerText_;
	Transform timerTextTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {640.0f, 600.0f, 0.0f} };

	GameDisplayRange displayRange_ = {};
};
