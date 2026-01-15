#pragma once
#include<unordered_map>
#include <deque>
#include"Common/KeyConfig/KeyManager.h"

#include"Unit/Player/PlayerUnit.h"
#include"Unit/Ore/OreUnit.h"

class UnitManager {
public:

	void Initalize(MapChipField* mapChipField, DrawData playerDrawData, DrawData oreDrawData, KeyManager* keyManager);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// ユニットを追加
	void AddOreUnit(const Vector3& pos);

private:

	// マップ
	MapChipField* mapChipField_ = nullptr;

	// おれの描画データ
	DrawData oreDrawData_;

	// プレイヤー
	std::unique_ptr<PlayerUnit> playerUnit_;

	// おれ
	// <id, インスタンス>
	std::unordered_map<int32_t,std::unique_ptr<OreUnit>> oreUnits_;
	// 最大のおれの数
	int32_t maxOreCount_ = 100;
	// 現在のid
	int32_t currentId_ = 0;
	// 再利用可能id
	std::deque<int32_t> freeIndices_;
};