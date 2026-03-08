#pragma once

enum class WeaponRarity : int {
	Common = 0,      // コモン
	Uncommon = 1,    // アンコモン
	Rare = 2,        // レア
	Epic = 3,        // エピック
};

enum class WeaponType : int {
	Pistol = 0,      // ピストル
	Sowrd = 1,       // ソード
};

struct WeaponData {
	int id;                   // ID
	WeaponType type;          // 武器の種類
	int rarity;               // レア度（WeaponRarityのint値）
	float gold;               // 値段
	float baseDamage;         // 基本ダメージ数
	float criticalChance;     // クリティカル発生確率
	float criticalMultiplier; // クリティカルダメージ倍率
	float lifeStealChance;    // ライフスティール確率
	float attackSpeed;        // 攻撃速度
	float knockbackPower;     // ノックバックの強さ
	float range;              // 射程距離
	int penetration;          // 貫通
	float spreadAngle;        // 拡散角度
	int size;                 // バックパック上でのサイズ
	char nameBuffer[64];      // ImGuiのInputText用の固定長のバッファ
};
