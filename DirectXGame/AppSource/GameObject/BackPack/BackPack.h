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
class DrawBackPack;



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

	// バックパックの描画部分
	std::unique_ptr<DrawBackPack> drawBackPack_;

	// バックパックのデータ部分
	std::vector<std::vector<std::unique_ptr<BackPackPiece>>> pieces_;

	// ショップアイテムのデータと描画部分
	std::unique_ptr<ItemLineup> itemLineup_;


};