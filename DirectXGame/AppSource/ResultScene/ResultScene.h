#pragma once
#include <Scene/IScene.h>
#include "Card/CardManager.h"
#include "Total/TotalManager.h"
#include <Render/PostEffect.h>
#include <ModelEditScene/Render/MapRender.h>
#include <ModelEditScene/Render/StaticObjectRender.h>
#include <Game/MiniMap/MiniMap.h>

enum class ResultState {
	Result,
	Total
};

class ResultScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void CameraUpdate(float deltaTime);

	bool resultFirstFinished_ = false;

	std::unique_ptr<Camera> camera_;
	Vector3 cameraPos_ = { -740.0f, 0.0f, 0.0f };
	std::unique_ptr<CardManager> cardManager_;

	std::unique_ptr<TotalManager> totalManager_;

	ResultState state;

	std::unique_ptr<PostEffect> bgpe_ = nullptr;
	PostEffectConfig bgConfig{};
	HeavyBlur blur_{};
	std::unique_ptr<PostEffect> pe_ = nullptr;
	PostEffectConfig config{};
	Fade fade_{};

	NewMap prevMap_{};
	std::unique_ptr<MapRender> mapRender_ = nullptr;
	std::unique_ptr<StaticObjectRender> staticObjectRender_ = nullptr;
	std::unique_ptr<MiniMap> miniMap_ = nullptr;

	bool fadeIn_ = true;
	bool fadeOut_ = false;

	bool MapInit_ = false;
};
