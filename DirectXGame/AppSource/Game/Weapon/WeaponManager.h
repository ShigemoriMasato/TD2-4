#pragma once
#include <unordered_map>
#include "WeaponData.h"
#include <Tool/Binary/BinaryManager.h>

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

	// バイナリに保存する
	void SaveWeaponData();

	// バイナリから読み込む
	void LoadWeaponData();

private:
	// 読み込み用のバイナリマネージャ
	BinaryManager binaryManager_;

	static inline const std::string weaponDataFile_ = "WeaponData.bin";
};
