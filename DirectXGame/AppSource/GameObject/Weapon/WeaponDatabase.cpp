#include "WeaponDatabase.h"
#include <../Engine/Tool/Json/JsonManager.h>

// -------------------------------------------------------------------
// | index | field               | type        | description         |
// |-------|---------------------|-------------|---------------------|
// | 0     | id                  | int         | 一意のID             |
// | 1     | rarity              | int         | レア度               |
// | 2     | gold                | float       | 値段                 |
// | 3     | baseDamage          | float       | 基本ダメージ           |
// | 4     | criticalChance      | float       | クリティカル発生確率    |
// | 5     | criticalMultiplier  | float       | クリティカルダメージ倍率 |
// | 6     | lifeStealChance     | float       | ライフスティール確率    |
// | 7     | attackSpeed         | float       | 攻撃速度(秒)           |
// | 8     | knockbackPower      | float       | ノックバックの強さ      |
// | 9     | range               | float       | 射程距離              |
// | 10    | penetration         | int         | 貫通                  |
// | 11    | spreadAngle         | float       | 拡散角度              |
// | 12    | size                | int         | バックパック上でのサイズ |
// | 13    | attackCount         | int         | 攻撃回数              |
// -------------------------------------------------------------------

std::vector<WeaponData> WeaponDatabase::GetDefaultWeaponData() {
	return {
		{0, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 6.0f,  0.5f, 2.0f, 0.2f, 1.5f, 1.0f, 1.0f, 0, 0.0f, 4, 1},      // ショートソード
		{1, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 10.0f, 0.5f, 2.0f, 0.2f, 1.8f, 1.0f, 1.0f, 0, 0.0f, 5, 1},      // ロングソード
		{2, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 4.0f,  0.5f, 2.0f, 0.2f, 2.2f, 1.0f, 1.0f, 0, 0.0f, 2, 1},      // ダガー
		{3, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 20.0f, 0.5f, 2.0f, 0.2f, 0.6f, 1.0f, 1.0f, 0, 0.0f, 6, 1},      // ハンマー
		{4, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 15.0f, 0.5f, 2.0f, 0.2f, 2.0f, 1.0f, 1.0f, 0, 0.0f, 5, 1},      // ウォーハンマー
		{5, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 2.0f,  0.5f, 2.0f, 0.2f, 0.2f, 1.0f, 1.0f, 0, 0.0f, 2, 1},      // 投げ斧
		{6, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 5.0f,  0.5f, 2.0f, 0.2f, 1.8f, 1.0f, 1.0f, 0, 0.0f, 3, 1},      // ハンドガン
		{7, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 14.0f, 0.5f, 2.0f, 0.2f, 0.7f, 1.0f, 1.0f, 0, 0.0f, 4, 1},      // ショットガン
		{8, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 8.0f,  0.5f, 2.0f, 0.2f, 1.2f, 1.0f, 1.0f, 0, 0.0f, 4, 1},      // ライフル
		{9, WeaponType::Pistol, static_cast<int>(WeaponRarity::Common), 20.0f, 8.0f,  0.5f, 2.0f, 0.2f, 1.2f, 1.0f, 1.0f, 0, 0.0f, 4, 1},      // バット
	};
}

float WeaponDatabase::GetRarityMultiplier(int rarity) {
	switch (rarity) {
		case static_cast<int>(WeaponRarity::Common):    return 1.0f;
		case static_cast<int>(WeaponRarity::Uncommon):  return 1.2f;
		case static_cast<int>(WeaponRarity::Rare):      return 1.4f;
		case static_cast<int>(WeaponRarity::Epic):      return 1.6f;
		default:                                       return 2.0f;
	}
}

void WeaponDatabase::Initialize(JsonManager& jsonManager) {
	// デフォルトデータで初期化
	database_ = GetDefaultWeaponData();

	// Jsonからロード
	LoadFromJson(jsonManager);

	isInitialized_ = true;
}

void WeaponDatabase::LoadFromJson(JsonManager& jsonManager) {
	for (size_t i = 0; i < database_.size(); ++i) {
		WeaponData& weaponData = database_[i];

		// Jsonファイルをロード
		jsonManager.Boot("WeaponData" + std::to_string(weaponData.id));

		try {
			// 基本情報を読み込む
			int id = jsonManager.Get<int>("id");
			int type = 0;
			int rarity = jsonManager.Get<int>("rarity");

			// レア度倍率を計算
			float rarityMultiplier = GetRarityMultiplier(rarity);

			// パラメータを読み込み、レア度倍率を適用
			weaponData.id = id;
			weaponData.type = static_cast<WeaponType>(type);
			weaponData.rarity = rarity;
			weaponData.gold = jsonManager.Get<float>("gold");
			weaponData.baseDamage = jsonManager.Get<float>("baseDamage") * rarityMultiplier;
			weaponData.criticalChance = jsonManager.Get<float>("criticalChance");
			weaponData.criticalMultiplier = jsonManager.Get<float>("criticalMultiplier");
			weaponData.lifeStealChance = jsonManager.Get<float>("lifeStealChance");
			weaponData.attackSpeed = jsonManager.Get<float>("attackSpeed");
			weaponData.knockbackPower = jsonManager.Get<float>("knockbackPower") * rarityMultiplier;
			weaponData.range = jsonManager.Get<float>("range");
			weaponData.penetration = jsonManager.Get<int>("penetration");
			weaponData.spreadAngle = jsonManager.Get<float>("spreadAngle");
			weaponData.size = jsonManager.Get<int>("size");
			weaponData.attackCount = jsonManager.Get<int>("attackCount");

		} catch (const std::exception&) {
			// Jsonの読み込みに失敗した場合はデフォルト値を使用（既に設定済み）
		}
	}
}

WeaponData* WeaponDatabase::GetWeapon(int id) {
	for (auto& weapon : database_) {
		if (weapon.id == id) {
			return &weapon;
		}
	}
	return nullptr;
}

const WeaponData* WeaponDatabase::GetWeapon(int id) const {
	for (const auto& weapon : database_) {
		if (weapon.id == id) {
			return &weapon;
		}
	}
	return nullptr;
}