#pragma once
#include<vector>

#include"Object/OreItem.h"
#include"Object/GoldOre.h"

#include"Object/OreOutLineObject.h"

class OreItemManager {
public:

	void Initialize(const DrawData& goldOreDrawData,int texture);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	void AddOreItem(OreType type, const Vector3& pos);

	/// <summary>
	/// 選択した位置に鉱石が存在しているかを取得
	/// </summary>
	/// <param name="selectpos">選択する位置</param>
	/// <param name="worldPos">選択した鉱石の位置</param>
	/// <returns></returns>
	bool IsSelectOre(const Vector3 selectpos,Vector3& worldPos);

	// 選択されたIdから鉱石を取得する
	OreItem* GetOreItemForId();

private:

	// 鉱石の描画データ
	DrawData goldOreDrawData_;
	int oreTexture_ = 0;

	// 鉱石の達
	std::unordered_map<int32_t, std::unique_ptr<OreItem>> oreItems_;
	// 現在のid
	int32_t currentId_ = 0;

	// 鉱石の削除リスト
	std::vector<std::pair<std::unique_ptr<OreItem>, int>> graveyard_;

	// 選択されているidを保持
	int32_t selectId_ = 0;

	// 選択中のオブジェクトにアウトラインを描画する
	std::unique_ptr<OreOutLineObject> oreOutLineObject_;
};