#pragma once
#include <Render/Font/Text.h>
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include <Common/KeyConfig/KeyManager.h>

/// <summary>
/// クリアタイマーを表示するためのクラス
/// </summary>
class ResultTimerText {
public:
	void Initialize(SHEngine::DrawData& drawData, float clearTimer, KeyManager* keyManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::Text> text_;
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;
	Transform spriteTransform_;
	AnimationBundle<float> scaleBeforeAnim_;
	AnimationBundle<float> scaleAfterAnim_;
	KeyManager* keyManager_;
	float spriteWidth_ = 300.0f;
	float spritePosX_ = 0.0f;
	bool isTextRender_ = false;
};
