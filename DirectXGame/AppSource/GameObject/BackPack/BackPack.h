#pragma once
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <GameObject/ScreenRaycaster/ScreenRaycaster.h>
#include "scene/CommonData.h"
#include "GameObject/Item/Item.h"
#include "GameObject/BackPack/GameConstants.h"

class ItemManager;
class BackPackPiece;



/// <summary>
/// 描画部分が膨れすぎたので一旦分割
/// </summary>
class DrawBackPack
{
public:
	DrawBackPack();
	~DrawBackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(const Matrix4x4& viewProj, const std::vector<std::vector<std::unique_ptr<BackPackPiece>>>& grids);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();

private:
	int textureIndex_ = 0;
	DirectionalLight light_{};


	// ロック中・解放不可
	std::unique_ptr<SHEngine::RenderObject> lockedUnavailableGrid_;
	std::vector<Matrix4x4> lockedUnavailableWorlds_;
	Vector4 lockedUnavailableColor_ = { 0.25f, 0.25f, 0.25f, 1.0f };
	InstanceBinding bindLockedUnavailable_{};

	// ロック中・解放可能
	std::unique_ptr<SHEngine::RenderObject> lockedAvailableGrid_;
	std::vector<Matrix4x4> lockedAvailableWorlds_;
	Vector4 lockedAvailableColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
	InstanceBinding bindLockedAvailable_{};

	// 解放済み
	std::unique_ptr<SHEngine::RenderObject> unlockedEmptyGrid_;
	std::vector<Matrix4x4> unlockedEmptyWorlds_;
	Vector4 unlockedEmptyColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	InstanceBinding bindUnlockedEmpty_{};

	// 毎フレーム転送（3重バッファ対策）
	void UpdateRenderObject(
		SHEngine::RenderObject& ro,
		const InstanceBinding& bind,
		const Matrix4x4& viewProj,
		const std::vector<Matrix4x4>& worlds,
		const Vector4& color);
};

struct LineupItemData
{
	// なんのアイテムか
	Item item;
	// 描画オブジェクト
	std::unique_ptr<SHEngine::RenderObject> renderObject;
	// 色
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// tier
	int rank = 0;

	// 初期位置
	Vector3 InitPos{};
	// マウスでホバーしているときの位置
	Vector3 hoverPos{};
	
	Matrix4x4 wvp{};
};

class ItemLineup
{
public:
	ItemLineup();
	~ItemLineup();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData);
	void RandomPickup();	// ラインナップ更新の度に呼ぶ
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();




private:
	// ラインナップするアイテムの数
	int lineupSum = 5;
	// ラインナップするアイテム
	std::vector<LineupItemData> lineupItems_;

	// マウスレイ
	std::unique_ptr<ScreenRaycaster> screenRaycaster_;
	Ray mouseRay_;

	// アイテム一覧ポインタ
	ItemManager* itemManager_ = nullptr;
	// モデルマネージャーポインタ
	SHEngine::ModelManager* modelManager_ = nullptr;
	// ドロー管理マネージャーポインタ
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	// 共通データポインタ
	CommonData* commonData_ = nullptr;

	bool IsCollision(const Ray& r, const AABB& aabb);
	std::optional<Vector3> IntersectRayAABB(const Ray& ray, const AABB& box);
};


/// <summary>
/// BackPackGridの集合体
/// </summary>
class BackPack
{
public:
	BackPack();
	~BackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData);
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();

private:

	// BackPackGridの描画部分
	std::unique_ptr<DrawBackPack> drawBackPack_;

	// BackPackGridのデータ部分
	std::vector<std::vector<std::unique_ptr<BackPackPiece>>> pieces_;

	// ショップアイテムのデータと描画部分
	std::unique_ptr<ItemLineup> itemLineup_;


};