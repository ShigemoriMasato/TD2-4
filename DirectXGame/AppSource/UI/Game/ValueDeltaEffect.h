#pragma once
#include "GameObject/EasingAnimation/EasingAnimation.h"
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class ValueDeltaEffect {
public:
	// 初期化処理
	void Initialize(SHEngine::DrawData& drawData, const std::string& fontPath, int fontSize = 64);

	// 通知を発生させるトリガー
	void Trigger(bool isIncrease, const Vector3& startPos);

	// 更新処理
	void Update(float deltaTime, const Matrix4x4& vpMat);

	// 描画処理
	void Draw(CmdObj* cmdObj);

	// 現在アニメーション中かどうか
	bool IsActive() const { return isActive_; }

private:
	std::unique_ptr<SHEngine::Text> text_;
	EasingAnimation<Vector3> posAnim_;
	EasingAnimation<float> alphaAnim_;

	bool isActive_ = false;
	Vector3 currentPos_{};
	float currentAlpha_ = 0.0f;
	Vector4 baseColor_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// 移動量とアニメーション時間
	const float moveOffsetY_ = 50.0f;
	const float animDuration_ = 0.5f;
};