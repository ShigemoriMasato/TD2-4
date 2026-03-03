#pragma once
#include "WeaponData.h"
#include <vector>
#include <memory>

class JsonManager;

/// <summary>
/// 武器データベース管理クラス
/// </summary>
class WeaponDatabase {
public:
	WeaponDatabase() = default;
	~WeaponDatabase() = default;

	// 初期化
	void Initialize(JsonManager& jsonManager);

	// IDから武器データを取得
	WeaponData* GetWeapon(int id);
	const WeaponData* GetWeapon(int id) const;

	// 全武器数を取得
	int GetWeaponCount() const { return static_cast<int>(database_.size()); }

	// 全武器データを取得
	const std::vector<WeaponData>& GetAllWeapons() const { return database_; }

	// 初期化済みかどうかを確認
	bool IsInitialized() const { return isInitialized_; }

private:
	std::vector<WeaponData> database_;
	bool isInitialized_ = false;

	// デフォルト武器データを取得
	static std::vector<WeaponData> GetDefaultWeaponData();

	// レア度に応じたパラメータ倍率を計算
	static float GetRarityMultiplier(int rarity);

	// Jsonからデータを読み込み、レア度を適用
	void LoadFromJson(JsonManager& jsonManager);
};
