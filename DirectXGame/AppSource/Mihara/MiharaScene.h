#pragma once
#include "Game/Player/Player.h"
#include "Game/Player/PlayerHP.h"
#include "Game/Weapon/WeaponDebugger.h"
#include "Game/Weapon/WeaponManager.h"
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
	std::unique_ptr<Camera> camera_ = nullptr;

	// 武器のパラメータ管理インスタンス
	std::unique_ptr<WeaponManager> weaponManager_ = nullptr;

	// 武器のパラメータ管理デバッガー 描画用インスタンス
	std::unique_ptr<WeaponDebugger> weaponDebugger_ = nullptr;

	// カメラのTransform
	Transform cameraTransform_{};
};
