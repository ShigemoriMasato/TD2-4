#pragma once
#include <GameObject/BackPack/GameConstants.h>
#include <GameObject/ScreenRaycaster/ScreenRaycaster.h>
#include "scene/CommonData.h"

class Shop
{
public:
	Shop();
	~Shop();
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

