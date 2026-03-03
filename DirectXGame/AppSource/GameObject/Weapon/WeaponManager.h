#pragma once
#include "WeaponData.h"
#include "WeaponDatabase.h"
#include <../Engine/Tool/Json/JsonManager.h>
#include <Tool/Binary/BinaryManager.h>
#include <memory>
#include <unordered_map>

/// <summary>
/// 武器のパラメータ管理用クラス
/// </summary>
class WeaponManager {
public:
	// データを初期化する
	void InitializeData();

	// IDから武器データを取得する
	WeaponData* GetWeapon(int id);
	const WeaponData* GetWeapon(int id) const;

	// Jsonに保存する
	void SaveWeaponData();

	// Jsonから読み込む
	void LoadWeaponData();

	// 武器データベースを取得
	WeaponDatabase& GetDatabase() { return weaponDatabase_; }
	const WeaponDatabase& GetDatabase() const { return weaponDatabase_; }

private:
	WeaponDatabase weaponDatabase_;
	std::unordered_map<int, WeaponData> database_;
	std::unique_ptr<JsonManager> jsonManager_ = nullptr;
};
