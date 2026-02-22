#pragma once
class WeaponManager;

/// <summary>
/// 武器のパラメータ管理デバッガー
/// </summary>
class WeaponDebugger {
private:
	WeaponManager* weaponManager_;
	int currentWeaponId = 0; // 現在のID

public:
	// コンストラクタ
	WeaponDebugger(WeaponManager* weaponManager) : weaponManager_(weaponManager) {}

	// 描画関数
	void Draw();
};
