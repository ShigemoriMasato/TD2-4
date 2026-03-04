#pragma once
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include "GameObject/BackPack/GameConstants.h"

class ItemManager;
class BackPackPiece;
class DrawBackPack;
class Shop;
class Item;
struct CommonData;

class BackPack
{
public:
	BackPack();
	~BackPack();
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData, SHEngine::Input* input);
	void Update(const Matrix4x4& viewProj);
	void Draw(SHEngine::Command::Object* cmdObject);
	void DrawImGui();
	

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mapIndex"> バックパック全体のインデックス</param>
	/// <param name="item"> 設置するアイテム</param>
	/// <param name="itemlocalShapeIndex"> アイテムの形状データのインデックス</param>
	/// <returns></returns>
	bool SetItem(Vector2int mapIndex, Item* item, const int itemlocalShapeIndex);

	// 座標からインデックスを取得する関数
	Vector2int GetIndexByPosition(const Vector3& pos) const;
	// インデックスから座標を取得する関数
	Vector3 GetPositionByIndex(const Vector2int& index) const;

	// 指定インデックスのピースが空いているか
	bool IsEmpty(const Vector2int& index) const;
	// 指定座標のピースが空いているか
	bool IsEmpty(const Vector3& pos) const;

private:

	// バックパックの描画部分
	std::unique_ptr<DrawBackPack> drawBackPack_;

	// バックパックのデータ部分
	std::vector<std::vector<std::unique_ptr<BackPackPiece>>> pieces_;
};