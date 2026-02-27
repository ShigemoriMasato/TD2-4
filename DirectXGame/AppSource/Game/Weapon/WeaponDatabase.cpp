#include "WeaponDatabase.h"

// ------------------------------------------------------------
// | index | field        | type        | description         |
// |-------|--------------|-------------|----------------------|
// | 0     | id           | int         | 一意のID            |
// | 1     | baseDamage   | float       | 基本ダメージ        |
// | 2     | attackSpeed  | float       | 攻撃速度            |
// | 3     | size         | int         | サイズ              |
// ------------------------------------------------------------

const WeaponData kWeaponDatabase[] = {
    {0, 6.0f,  1.5f, 4}, // ショートソード
    {1, 10.0f, 1.8f, 5}, // ロングソード
    {2, 4.0f,  2.2f, 2}, // ダガー
    {3, 20.0f, 0.6f, 6}, // ハンマー
    {4, 15.0f, 2.0f, 5}, // ウォーハンマー
    {5, 2.0f,  0.2f, 2}, // 投げ斧
    {6, 5.0f,  1.8f, 3}, // ハンドガン
    {7, 14.0f, 0.7f, 4}, // ショットガン
    {8, 8.0f,  1.2f, 4}, // ライフル
};

const int kWeaponDatabaseCount = sizeof(kWeaponDatabase) / sizeof(WeaponData);