#pragma once
#include <Scene/IScene.h>
#include <GameObject/Item/ItemManager.h>
#include <Collision/ColliderManager.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include <GameObject/ObjectRender.h>
#include <Shop/BackPack.h>
#include <Shop/Piece.h>
#include <Shop/ShopCursor.h>
#include <Shop/PieceManager.h>
#include <Shop/Shop.h>

class ShopScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

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
};
