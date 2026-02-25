#pragma once
#include <string>
#include <unordered_map>

struct WeaponData {
	int id;              // ID
	std::string name;    // 名前
	float baseDamage;    // 基本ダメージ数
	float attackSpeed;   // 攻撃速度
	float price;         // 値段
	int size;            // バックパック上でのサイズ
	int tier;            // レア度
	char nameBuffer[64]; // ImGuiのInputText用の固定長のバッファ
};