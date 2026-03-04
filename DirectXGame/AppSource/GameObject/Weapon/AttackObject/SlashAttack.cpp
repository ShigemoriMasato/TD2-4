#include "SlashAttack.h"
#include "../AppSource/GameObject/Random/Random.h"

SlashAttack::SlashAttack(const WeaponData& weaponData) {
	float finalDamage=weaponData.baseDamage;

	// クリティカル判定
	float rand = RandomUtils::RangeFloat(0.0f,1.0f);

	// ダメージ値を設定
	damageInfo_.damage=finalDamage;

	// 貫通回数を設定
	penetrationCount_=weaponData.penetration;

	// 斬撃が存在する時間
	lifeTime_=0.2f;
}

void SlashAttack::Update(float deltaTime) {
	// 寿命を減らす
	lifeTime_-=deltaTime;

	// 斬撃を移動させる処理

}

void SlashAttack::Draw(CmdObj* cmdObj) {
	// 斬撃の描画

}
