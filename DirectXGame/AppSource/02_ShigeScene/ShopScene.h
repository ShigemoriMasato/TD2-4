#pragma once
#include <Scene/IScene.h>
#include <GameObject/Item/ItemManager.h>
#include <GameObject/Weapon/WeaponManager.h>
#include <GameObject/Weapon/WeaponDebugger.h>
#include <Collision/ColliderManager.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include <GameObject/ObjectRender.h>
#include <Shop/BackPack.h>
#include <Shop/Piece.h>
#include <Shop/ShopCursor.h>
#include <Shop/PieceManager.h>
#include <Shop/Shop.h>
#include <GameObject/Player/Parameter/ParameterRender.h>
#include <Shop/ShopDisplay.h>

class ShopScene : public IScene {
public:

	~ShopScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void DrawReady();
	void Draw() override;
	void SetDeltaTime(float& deltaTime) { deltaTime_ = deltaTime; }
	ItemManager* GetItemManager() { return itemManager_.get(); }
	DebugCamera* GetCamera() { return debugCamera_.get(); }

private:

	std::unique_ptr<ColliderManager> colliderManager_;
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<Grid> grid_;
	std::unique_ptr<ObjectRender> objectRender_;

	std::unique_ptr<ItemManager> itemManager_;
	std::unique_ptr<BackPack> backPack_;
	std::unique_ptr<ShopCursor> shopCursor_;
	std::unique_ptr<PieceManager> pieceManager_;
	std::unique_ptr<Shop> shop_;

	std::unique_ptr<WeaponManager> weaponManager_;
	std::unique_ptr<WeaponDebugger> weaponDebugger_;

	std::unique_ptr<ParameterRender> parameterRender_;
	std::unique_ptr<Camera> orthoCamera_;

	std::unique_ptr<ShopDisplay> shopDisplay_;

	std::unique_ptr<SHEngine::RenderObject> debugObj_;
	Transform debugTransform_;
	Vector4 debugColor_ = { 0.0f, 0.0f, 0.0f, 0.1f };

	float deltaTime_ = 0.0f;

	bool useAutoReroll_ = true;
	float shopRerollTime_ = 5.0f;
	float shopRerollTimer_ = 0.0f;
};