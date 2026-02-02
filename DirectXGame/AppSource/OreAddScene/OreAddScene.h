#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <UI/Object/FontObject.h>
#include <UI/Number.h>
#include <Render/PostEffect.h>

class OreAddScene : public IScene {
public:

	OreAddScene() = default;
	~OreAddScene() override = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	bool Wait(float deltaTime, float targetTime);
	bool OreAdd(float deltaTime);

private: //Camera

	std::unique_ptr<Camera> camera_ = nullptr;

private: //phase

	int phase_ = 0;

private: //font
	
	std::unique_ptr<FontObject> result_ = nullptr;
	std::unique_ptr<FontObject> oreNumIs_ = nullptr;
	std::unique_ptr<Number> oreNum_ = nullptr;
	std::unique_ptr<FontObject> goldNumIs_ = nullptr;
	std::unique_ptr<Number> goldNum_ = nullptr;

	std::unique_ptr<FontObject> slashNumGold_ = nullptr;
	std::unique_ptr<FontObject> slashNumOre_ = nullptr;

private: //postEffect

	std::unique_ptr<PostEffect> postEffect_ = nullptr;
	PostEffectConfig postEffectConfig_{};
	Fade fade_{};
	Blur blur_{};

	//鉱石 n個 でOre一個増やす
	int rateGoldToOre_ = 2;

	float fadeTimer_ = 0.0f;
	const float kFadeDuration_ = 1.0f;
};
