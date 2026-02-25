#pragma once
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

private:
	// モデルデータをバイナリにセーブする関数
	void SaveModel();

	// モデルデータをバイナリからロードする関数
	void LoadModel();

	// 武器データをバイナリにセーブする関数
	void SaveItem();

	// 武器データをバイナリからロードする関数
	void LoadItem();

	// 武器のパラメータをバイナリにセーブする関数
	void SaveBaseParam();

	// モデルデータをバイナリにセーブする関数
	void LoadBaseParam();

private:
	// バイナリマネージャ
	BinaryManager binaryManager_;
};
