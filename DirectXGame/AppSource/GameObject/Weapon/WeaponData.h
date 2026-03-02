#pragma once

struct WeaponData {
	int id;              // ID
	float baseDamage;    // 基本ダメージ数
	float attackSpeed;   // 攻撃速度
	int size;            // バックパック上でのサイズ
	char nameBuffer[64]; // ImGuiのInputText用の固定長のバッファ
};