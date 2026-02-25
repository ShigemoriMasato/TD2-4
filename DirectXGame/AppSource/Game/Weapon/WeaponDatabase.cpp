#include "WeaponDatabase.h"

// ------------------------------------------------------------
// | index | field        | type        | description         |
// |-------|--------------|-------------|----------------------|
// | 0     | id           | int         | 一意のID            |
// | 1     | name         | string      | 武器名              |
// | 2     | baseDamage   | float       | 基本ダメージ        |
// | 3     | attackSpeed  | float       | 攻撃速度            |
// | 4     | price        | float       | 値段                |
// | 5     | size         | int         | サイズ              |
// | 6     | tier         | int         | レア度              |
// ------------------------------------------------------------

const WeaponData kWeaponDatabase[] = {
    {0, "Short_Sword", 6.0f,  1.5f, 50.0f, 4, 1}, // ショートソード
    {1, "Long_Sword",  10.0f, 1.8f, 120.0f, 5, 2}, // ロングソード
    {2, "Dagger",      4.0f,  2.2f, 30.0f, 2, 2}, // ダガー
    {3, "Hammer",      20.0f, 0.6f, 200.0f, 6, 2}, // ハンマー
    {4, "Warhammer",   15.0f, 2.0f, 100.0f, 5, 2}, // ウォーハンマー
    {5, "Throw_Ax",    2.0f,  0.2f, 30.0f, 2, 2}, // 投げ斧
    {6, "Handgun",     5.0f,  1.8f, 90.0f, 3, 2}, // ハンドガン
    {7, "Shotgun",     14.0f, 0.7f, 180.0f, 4, 2}, // ショットガン
    {8, "Rifle",       8.0f,  1.2f, 160.0f, 4, 2}, // ライフル
};

const int kWeaponDatabaseCount = sizeof(kWeaponDatabase) / sizeof(WeaponData);