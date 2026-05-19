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
#include <02_ShigeScene/GameDisplayRange.h>
#include <UI/Game/SituationGauge.h>
#include <UI/Game/GaugeAttractEffect.h>
#include <UI/Game/ValueDeltaEffect.h>
#include <GameObject/Enemy/EnemyEffect/EnemyEffectManager.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

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
	Vector2 rerollBarSize_ = { 850.0f, 50.0f };
	Vector2 rerollBarPos_ = { 480.0f, -230.0f };
	
	// リロールテキスト用変数
	std::unique_ptr<SHEngine::Text> rerollText_ = nullptr;
	Transform rerollTextTransform_ = { {3.0f, 1.5f, 1.0f}, {0.0f, 0.0f, 0.0f}, {70.0f, -245.0f, 0.0f} };
	Vector4 rerollTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 操作説明テキスト用変数
	std::unique_ptr<SHEngine::Text> controlText_ = nullptr;
	Transform controlTextTransform_ = { {2.0f, 1.5f, 1.0f}, {0.0f, 0.0f, 0.0f}, {190.0f, -315.0f, 0.0f} };
	Vector4 controlTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// ラクラク配置テキスト用変数
	std::unique_ptr<SHEngine::Text> easyPlaceText_ = nullptr;
	Transform easyPlaceTextTransform_ = { {2.0f, 1.5f, 1.0f}, {0.0f, 0.0f, 0.0f}, {610.0f, -315.0f, 0.0f} };
	Vector4 easyPlaceTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// 武器安置所テキスト用変数
	std::unique_ptr<SHEngine::Text> weaponStorageText_ = nullptr;
	Transform weaponStorageTextTransform_ = { {3.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {65.0f, -612.0f, 0.0f} };
	Vector4 weaponStorageTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// マウスボタンスプライト用変数
	std::unique_ptr<SHEngine::RenderObject> mouseLeftSprite_ = nullptr;
	std::unique_ptr<SHEngine::RenderObject> mouseRightSprite_ = nullptr;
	Transform mouseLeftTransform_ = { {150.0f, 50.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {550.0f, -300.0f, 0.0f} };
	Transform mouseRightTransform_ = { {150.0f, 50.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {125.0f, -300.0f, 0.0f} };
	int mouseLeftTextureIndex_ = -1;
	int mouseRightTextureIndex_ = -1;
	int mouseLeftActiveTextureIndex_ = -1;
	int mouseRightActiveTextureIndex_ = -1;

	Vector3 cameraCenter_ = { -7.0f, -40.0f, -6.5f };
	Vector3 cameraSpherical_ = { 20.0f, 0.0f, -1.570f };
	Vector2 cameraPerspectiveSize_ = { 352.0f, 624.0f };
	
	int pieceModelID_ = -1;

	int rerollCount_ = 3; // リロール可能な回数
	bool pendingReroll_ = false; // リロール待機フラグ
	float rerollIntervalTime_ = 1.5f; // 補充インターバル時間（秒）
	float rerollIntervalTimer_ = 0.0f; // 補充インターバルタイマー

	GameDisplayRange displayRange_;
	
	// 有利不利ゲージ
	std::unique_ptr<SituationGauge> situationGauge_;

	// 武器配置時に発生するエフェクト
	std::vector<std::unique_ptr<GaugeAttractEffect>> attractEffects_;

	Vector3 effectEndPos_ = {1050, -600.0f, 0.0f};
	Vector3 control1_ = {200.0f, 200.0f, 0.0f}; // 制御点1
	Vector3 control2_ = {400.0f, -200.0f, 0.0f};  // 制御点2

	// 増減エフェクト
	std::vector<std::unique_ptr<ValueDeltaEffect>> valueEffects_;
	SHEngine::DrawData textDrawData_{};
	Vector3 valueEfectPos_ = {1000.0f, -200.0f, 0.0f};

	// pieceBreakパーティクル
	std::unique_ptr<ParticleDrawer> shopParticleDrawer_;
	std::unordered_map<int, MultiParticleData> breakParticles_;
	int nextBreakParticleId_ = 0;
};
