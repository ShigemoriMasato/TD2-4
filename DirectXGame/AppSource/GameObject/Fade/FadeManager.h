#pragma once
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Scene/IScene.h>

class FadeManager {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	// フェードアウト開始
	void StartFadeOut(bool notifyFinish = true);

	// フェードイン開始
	void StartFadeIn(std::function<std::unique_ptr<IScene>()> nextScene, bool notifyFinish = true);

	// フェードの更新
	void UpdateFade(float deltaTime);

	// フェードが終了したかどうか
	bool Finished() { return isFinished_; }

	// シーンを受け取る
	std::unique_ptr<IScene> TakeNextScene();

private:
	enum class FadeState {
		Idle,
		FadeIn,
		FadeOut,
	};

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;
	float alpha_ = 0.0f;
	float fadeSpeed_ = 1.0f;
	FadeState state_ = FadeState::Idle;
	bool isFinished_ = false;
	bool notifyFinish_ = false;

	// 遷移先シーン
	std::function<std::unique_ptr<IScene>()> nextScene_ = nullptr;
};
