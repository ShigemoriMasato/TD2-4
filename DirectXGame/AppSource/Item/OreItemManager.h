#pragma once
#include<vector>

#include"Object/OreItem.h"
#include"Object/GoldOre.h"

#include"Object/OreOutLineObject.h"
#include"UI/Object/FontObject.h"

class OreItemManager {
public:

	void Initialize(const DrawData& goldOreDrawData,int texture, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void DrawUI();

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

	// 現在の鉱石の数
	int32_t GetCurrentOreItemNum() { return static_cast<int32_t>(oreItems_.size()); }

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

private: // フォント機能

	FontLoader* fontLoader_ = nullptr;
	DrawData fontDrawData_;

	// フォントデータ
	std::unordered_map<int32_t, std::unique_ptr<FontObject>> fontList_;

	std::string fontName_;
};