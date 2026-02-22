#pragma once
#include "Game/Player/Player.h"
#include "Game/Player/PlayerHP.h"
#include <Camera/Camera.h>
#include <Scene/IScene.h>

class MiharaScene : public IScene {
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	// プレイヤーのHP
	std::unique_ptr<PlayerHP> playerHP_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_;

	// カメラのTransform
	Transform cameraTransform_{};
};
