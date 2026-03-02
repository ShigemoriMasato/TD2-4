#pragma once

#ifdef USE_IMGUI

class ItemManager;

class ItemEditor
{
public:
	void Draw(ItemManager& itemManager);

private:
	// 現在選択しているアイテムのインデックス
	int currentItemIndex_ = 0;
	// 前回選択していたアイテムのインデックス 
	int lastItemIndex_ = -1;

	// 新規追加する時の名前バッファ
	char newItemName_[64] = "Default Item";
	// 新規追加する時のカテゴリバッファ（0:Weapon 1:Armor 2:Item）
	int newItemCategory_ = 2;
	// 既存アイテムの名前編集用バッファ
	char editItemName_[64] = {};

	// アイテム追加した時の状態 0:成功 1:重複 2:空
	int addItemState_ = 0;
	// アイテム名変更したときの状態 0:成功 1:重複 2:空
	int renameItemState_ = 0;

	// モデルパス編集用バッファ
	char editModelPath_[256] = {};

	//　mapData編集用
	int gridW_ = 8;
	int gridH_ = 8;
	float cellSize_ = 16.0f;
	int paintMode_ = 0; // 0:none 1:add(L) 2:erase(R)
};

#else

class ItemManager;
class ItemEditor
{
public:
	void Draw(ItemManager&) {}
};

#endif