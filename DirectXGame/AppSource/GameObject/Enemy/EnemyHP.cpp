#include "EnemyHP.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

using namespace SHEngine;

void EnemyHP::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	// 単位行列の代入
	hpBarFill_.wvp = Matrix4x4::Identity();
	hpBarAfter_.wvp = Matrix4x4::Identity();
	hpBarBG_.wvp = Matrix4x4::Identity();

	// 前面
	hpBarFill_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarFill_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarFill_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// 減った分
	hpBarAfter_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarAfter_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarAfter_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// 背景
	hpBarBG_.transform.scale = {hpBarSize_.x, hpBarSize_.y, 1.0f};
	hpBarBG_.transform.rotate = {0.0f, 0.0f, 0.0f};
	hpBarBG_.transform.position = {hpBarPos_.x, hpBarPos_.y, 0.0f};

	// モデルマネージャー
	modelManager_ = modelManager;
}

void EnemyHP::Update(float deltaTime, float currentHP, float maxHP, const Vector3& enemyScreenPos) {
	hpBarPos_ = enemyScreenPos;
	hpBarPos_.y += 3.0f;

	hpBarFill_.transform.position = hpBarPos_;
	hpBarAfter_.transform.position = hpBarPos_;
	hpBarBG_.transform.position = hpBarPos_;

	if (previousHP_ != currentHP) {
		HPBarScaleChange(currentHP, maxHP);
		previousHP_ = currentHP;
	}

	AnimationHPBarAfter(deltaTime);

	// X位置を調整しつつ、Y位置も最新の座標に合わせる
	float offsetX = (hpBarSize_.x - hpBarFill_.transform.scale.x) / 2.0f;
	hpBarFill_.transform.position.x = hpBarPos_.x - offsetX;

	// 背景とAfterのY座標も合わせる
	float afterOffsetX = (hpBarSize_.x - hpBarAfter_.transform.scale.x) / 2.0f;
	hpBarAfter_.transform.position.x = hpBarPos_.x - afterOffsetX;
	hpBarBG_.transform.position.x = hpBarPos_.x;
}

void EnemyHP::HPBarScaleChange(float currentHP, float maxHP) {
	// 現在のHPの比率
	float hpRatio = currentHP / maxHP;

	// 新しいスケール
	float newScale = hpRatio * hpBarSize_.x;

	// HPバー　減った分のアニメーション用変数の初期化
	scaleAnimationHPBarAfter_.anim.Start(hpBarFill_.transform.scale.x, newScale, 1.0f, EaseType::EaseOutCubic);

	// 前面のスケール変更
	hpBarFill_.transform.scale.x = newScale;

	// X位置を調整してHPバーが左詰めに見えるようにする
	float offsetX = (hpBarSize_.x - hpBarFill_.transform.scale.x) / 2.0f;
	hpBarFill_.transform.position.x = hpBarPos_.x - offsetX;
	hpBarAfter_.transform.position.x = hpBarPos_.x - offsetX;
}

void EnemyHP::AnimationHPBarAfter(float deltaTime) {
	// スケールアニメーションの更新
	scaleAnimationHPBarAfter_.anim.Update(deltaTime, scaleAnimationHPBarAfter_.temp);

	// 変更した値の変更
	hpBarAfter_.transform.scale.x = scaleAnimationHPBarAfter_.temp;

	// 座標の変更
	float offsetX = (hpBarSize_.x - hpBarAfter_.transform.scale.x) / 2.0f;
	hpBarAfter_.transform.position.x = hpBarPos_.x - offsetX;
}

std::vector<DrawInfo> EnemyHP::GetDrawInfo() const {
	std::vector<DrawInfo> drawInfos;
	// 背景から前面の順に描画
	DrawInfo bgInfo;
	bgInfo.position = hpBarBG_.transform.position;
	bgInfo.rotation = hpBarBG_.transform.rotate;
	bgInfo.scale = hpBarBG_.transform.scale;
	bgInfo.modelIndex = 1;
	bgInfo.color = 0x000000ff;
	drawInfos.push_back(bgInfo);

	DrawInfo afterInfo;
	afterInfo.position = hpBarAfter_.transform.position;
	afterInfo.rotation = hpBarAfter_.transform.rotate;
	afterInfo.scale = hpBarAfter_.transform.scale;
	afterInfo.modelIndex = 1;
	afterInfo.color = 0xffff00ff;
	drawInfos.push_back(afterInfo);

	DrawInfo fillInfo;
	fillInfo.position = hpBarFill_.transform.position;
	fillInfo.rotation = hpBarFill_.transform.rotate;
	fillInfo.scale = hpBarFill_.transform.scale;
	fillInfo.modelIndex = 1;
	fillInfo.color = 0xff0000ff;
	drawInfos.push_back(fillInfo);

	return drawInfos;
}
