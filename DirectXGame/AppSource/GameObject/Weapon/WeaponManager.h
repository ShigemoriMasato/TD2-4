#pragma once
#include "WeaponData.h"
#include <../Engine/Tool/Json/JsonManager.h>
#include <Tool/Binary/BinaryManager.h>
#include <memory>
#include <unordered_map>

/// <summary>
/// 武器のパラメータ管理用クラス
/// </summary>
class WeaponManager {
public:
	std::unordered_map<int, WeaponData> database_;

	// データを追加する関数
	void InitializeData();

	// IDから武器データを取得する
	WeaponData* GetWeapon(int id);

	// Jsonに保存する
	void SaveWeaponData();

	// Jsonから読み込む
	void LoadWeaponData();

private:
	// JsonManager
	std::unique_ptr<JsonManager> jsonManager_ = nullptr;
};
