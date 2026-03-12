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
#include <GameObject/Enemy/WaveSystem.h>
#include <GameObject/GameTimer.h>
#include <GameObject/Player/Controller/AIController.h>
#include <GameObject/Player/Controller/InputController.h>
#include <02_ShigeScene/ShopScene.h>

class ShigeScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void MakeWeapon();

	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<GameCamera> gameCamera_;
	Camera* camera_;
	std::unique_ptr<Grid> grid_;
	std::unique_ptr<ColliderManager> colliderManager_;

	std::unique_ptr<Player::Base> player_;
	std::vector<std::unique_ptr<IWeapon>> weapons_;
	std::unique_ptr<Map> map_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<ObjectRender> objectRender_;
	std::vector<DrawInfo> drawInfos_;
	std::unique_ptr<WeaponDatabase> weaponDatabase_;
	std::unique_ptr<AttackManager> attackManager_;
	std::vector<std::unique_ptr<IWeaponRender>> weaponRenders_;
	std::unique_ptr<ItemManager> itemManager_;
	std::unique_ptr<WaveSystem> waveSystem_;
	std::unique_ptr<GameTimer> gameTimer_;
	std::vector<IController*> controllers_;
	std::unique_ptr<AIController> aiController_;
	std::unique_ptr<InputController> inputController_;

	//プレイヤーが持っているピースの生ポインタ。
	std::vector<Piece*> pieces_;

	JsonManager jsonManager_;

	float worldTimer_ = 0.0f;

	// 現在使用しているコントローラーのインデックス
	int currentControllerIndex_ = 0;

	// 武器の配置関連の変数
	float baseRadius_ = 4.0f; // 円の半径
	float baseHeight_ = 2.0f; // 高さ
	float baseRadius_ = 4.0f;
	float baseHeight_ = 2.0f;

private:// Shop

	std::unique_ptr<ShopScene> shopScene_;

};
