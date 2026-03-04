#pragma once
#include <GameObject/BackPack/GameConstants.h>
#include <GameObject/ScreenRaycaster/ScreenRaycaster.h>
#include "scene/CommonData.h"
#include "GameObject/Item/Item.h"
#include "GameObject/LineDrawer/PrimitiveLineDrawer.h" 

class ItemManager;
class BackPack;

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
	// 今ホバーされているか
	bool isHover = false;

	Matrix4x4 wvp{};
};


class Shop
{
public:
	Shop();
	~Shop();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData, SHEngine::Input* input, BackPack* backPack);
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();

	/// <summary>
	/// ラインナップ商品をランダムに選択して入れ替える関数
	/// </summary>
	void RandomPickup();

	/// <summary>
	/// 現在持っているアイテムを設置する
	/// </summary>
	bool CanPlaceHaveItem(LineupItemData& lineupData);
	

private:
	// ラインナップするアイテムの数
	int lineupSum = 5;
	// ラインナップするアイテム
	std::vector<LineupItemData> lineupItems_;
	// 現在持っているアイテム
	std::optional<Item> haveItem_;

	// マウスレイ
	std::unique_ptr<ScreenRaycaster> screenRaycaster_;
	Ray mouseRay_;

	// デバッグ用：プリミティブのライン描画
	std::unique_ptr<PrimitiveLineDrawer> debugLineDrawer_;



	// バックパックのポインタ
	BackPack* backPack_ = nullptr;

	// アイテム一覧ポインタ
	ItemManager* itemManager_ = nullptr;
	// モデルマネージャーポインタ
	SHEngine::ModelManager* modelManager_ = nullptr;
	// ドロー管理マネージャーポインタ
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	// コモンデータポインタ
	CommonData* commonData_ = nullptr;
	// 入力システムポインタ
	SHEngine::Input* input_ = nullptr;

	bool IsCollision(const Ray& r, const AABB& aabb);
	std::optional<Vector3> IntersectRayAABB(const Ray& ray, const AABB& box);

	bool IsCollision(Ray& r, const PlaneXZ& plane);
};

