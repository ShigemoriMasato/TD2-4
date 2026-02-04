#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <UI/Object/FontObject.h>
#include <UI/Number.h>
#include <Render/PostEffect.h>
#include "Difficult.h"

class OreAddScene : public IScene {
public:

	OreAddScene() = default;
	~OreAddScene() override { Save(); };

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	bool Wait(float deltaTime, float targetTime);
	bool OreAdd(float deltaTime);

	void Save();
	void Load();

private: //Camera

	std::unique_ptr<Camera> camera_ = nullptr;
	Transform cameraTransform_{};

private: //phase

	int phase_ = 0;
	bool notDraw_ = false;

private: //font
	
	std::unique_ptr<FontObject> result_ = nullptr;
	std::unique_ptr<FontObject> oreNumIs_ = nullptr;
	std::unique_ptr<Number> oreNum_ = nullptr;
	std::unique_ptr<FontObject> goldNumIs_ = nullptr;
	std::unique_ptr<Number> goldNum_ = nullptr;

	std::unique_ptr<FontObject> slashNumGold_ = nullptr;
	std::unique_ptr<FontObject> slashNumOre_ = nullptr;

	std::unique_ptr<RenderObject> fukidashi_ = nullptr;
	std::vector<Transform> fukidashiTransforms_{};
	int fkdsTextureIndex_ = 0;
	std::unique_ptr<Number> addNum_ = nullptr;
	Transform addNumTransform_{};

private: //Endress

	std::unique_ptr<Difficult> difficulty_ = nullptr;

private: //postEffect

	std::unique_ptr<PostEffect> postEffect_ = nullptr;
	PostEffectConfig postEffectConfig_{};
	Fade fade_{};
	Blur blur_{};

	std::unique_ptr<PostEffect> endProcess_ = nullptr;
	PostEffectConfig endConfig_;

	//鉱石 n個 でOre一個増やす
	int rateGoldToOre_ = 2;

	float fadeTimer_ = 0.0f;
	const float kFadeDuration_ = 1.0f;

	int oreAddNum_ = 0;

	BinaryManager binaryManager_;
};
