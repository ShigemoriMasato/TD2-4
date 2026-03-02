#pragma once
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <Game/ScreenRaycaster/ScreenRaycaster.h>
#include "scene/CommonData.h"
#include "Game/Item/Item.h"

class ItemManager;

enum class GridState
{
	// ロック中・解放不可
	LockedUnavailable,
	// ロック中・解放可能
	LockedAvailable,
	// 解放済み・空
	UnlockedEmpty,
	// 解放済み・アイテムあり
	UnlockedOccupied,
};

struct InstanceBinding
{
	int matricesSrvIndex = -1; // VS t0
	int vpCbvIndex = -1;       // VS b0
	int texCbvIndex = -1;      // PS b0
	int colorCbvIndex = -1;    // PS b1
	int lightCbvIndex = -1;    // PS b2
};

namespace GameConstants
{
	// バックパックの行数
	inline constexpr size_t kBackPackRowNum = 8;
	// バックパックの列数
	inline constexpr size_t kBackPackColNum = 12;
}


/// <summary>
/// バックパックの1マス
/// </summary>
class BackPackGrid
{
public:
	BackPackGrid();
	~BackPackGrid();
	void Initialize(GridState state);
	void ChangeState(GridState newState);
	void Update();

	GridState GetState() const { return state_; }
	const Vector4& GetColor() const { return color_; }

	Transform transform_{};

private:
	Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	GridState state_ = GridState::LockedUnavailable;
};

/// <summary>
/// 描画部分が膨れすぎたので一旦分割
/// </summary>
class DrawBackPack
{
public:
	DrawBackPack();
	~DrawBackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(const Matrix4x4& viewProj, const std::vector<std::vector<std::unique_ptr<BackPackGrid>>>& grids);
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
	Item item;
	std::unique_ptr<SHEngine::RenderObject> renderObject;
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	int rank = 0;

	Transform transform{};
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
	std::vector<std::vector<std::unique_ptr<BackPackGrid>>> grids_;

	// ショップアイテムのデータと描画部分
	std::unique_ptr<ItemLineup> itemLineup_;


};