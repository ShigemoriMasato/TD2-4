#pragma once
#include <Scene/IScene.h>
#include "Card/CardManager.h"

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

	bool reusltFirstFinished_ = false;

	std::unique_ptr<Camera> camera_;
	Vector3 cameraPos_ = { -740.0f, 0.0f, 0.0f };
	std::unique_ptr<CardManager> cardManager_;
	std::unique_ptr<Card> debug_;

	ResultState state;
};
