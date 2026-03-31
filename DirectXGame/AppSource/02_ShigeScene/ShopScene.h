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
#include <Render/Font/Text.h>

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
	
	// 現在のWave数を設定
	void SetCurrentWave(int wave) { if (pieceManager_) pieceManager_->SetCurrentWave(wave); }

private:

	// リロールバーの構造体
	struct RerollBar {
		std::unique_ptr<SHEngine::RenderObject> render = nullptr;
		Matrix4x4 wvp;
		Transform transform;
	};

	// リロールバーの初期化
	void InitializeRerollBar();
	// リロールバーの更新
	void UpdateRerollBar(Matrix4x4 vpMatrix);
	// リロールバーの描画
	void DrawRerollBar(CmdObj* cmdObj);

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
	float shopRerollTime_ = 15.0f;
	float shopRerollTimer_ = 0.0f;

	// リロールバー用変数
	RerollBar rerollBarFill_;  // 前面（進行状況）
	RerollBar rerollBarBG_;    // 背景
	Vector2 rerollBarSize_ = { 950.0f, 50.0f };
	Vector2 rerollBarPos_ = { 640.0f, -210.0f };
	
	// リロールテキスト用変数
	std::unique_ptr<SHEngine::Text> rerollText_ = nullptr;
	Transform rerollTextTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {175.0f, -230.0f, 0.0f} };
	float rerollTextSize_ = 2.0f;
	Vector4 rerollTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	Vector3 cameraCenter_ = { -5.0f, -32.0f, -6.5f };
	Vector3 cameraSpherical_ = { 20.0f, 0.0f, -1.570f };
	Vector2 cameraPerspectiveSize_ = { 352.0f, 624.0f };
	
	int pieceModelID_ = -1;

	int rerollCount_ = 0; // リロール可能な回数
	bool pendingReroll_ = false; // リロール待機フラグ
};