#pragma once
#include "../AppSource/GameObject/Field/Field.h"
#include "../AppSource/GameObject/Player/Heatmap/PlayerHeatmap.h"
#include "../AppSource/GameObject/Player/Level/LevelProgressTracker.h"
#include "../AppSource/GameObject/Player/Level/PlayerLevelSystem.h"
#include "../AppSource/GameObject/Player/Level/PlayerLevelUI.h"
#include "GameObject/Player/Player.h"
#include "GameObject/Player/PlayerHP.h"
#include "GameObject/Weapon/WeaponDebugger.h"
#include "GameObject/Weapon/WeaponManager.h"
#include <Camera/Camera.h>
#include <Scene/IScene.h>
#include <Tool/Grid/Grid.h>

class MiharaScene : public IScene {
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	// デバッグ機能ImGui表示
	void DrawDebugUI();

	// カメラのImGui表示
	void DrawImGuiCamera();

	// ポーズ中ImGui表示
	void DrawImGuiPause();

private:
	// プレイヤー
	std::unique_ptr<Player::Base> player_ = nullptr;

	// プレイヤーのHP
	std::unique_ptr<Player::HP> playerHP_ = nullptr;

	// プレイヤーのレベルシステム
	std::unique_ptr<Player::LevelSystem> playerLevelSystem_ = nullptr;

	// プレイヤーのレベルUI
	std::unique_ptr<Player::LevelUI> playerLevelUI_ = nullptr;

	// ウェーブ中にどれくらいレベルが上がったかを管理するインスタンス
	std::unique_ptr<LevelProgressTracker> levelProgressTracker_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	// 武器のパラメータ管理インスタンス
	std::unique_ptr<WeaponManager> weaponManager_ = nullptr;

	// 武器のパラメータ管理デバッガー 描画用インスタンス
	std::unique_ptr<WeaponDebugger> weaponDebugger_ = nullptr;

	// プレイヤーのヒートマップ
	std::unique_ptr<Player::HeatmapManager> playerHeatmapManager_ = nullptr;

	// フィールド
	std::unique_ptr<Field> field_ = nullptr;

	// グリッド
	std::unique_ptr<Grid> grid_;

	// カメラのTransform
	Transform cameraTransform_{};

	// ポーズ状態を管理するフラグ
	bool isPaused_ = false;

	// グリッドのパラメータ
	GridConfig config_;

	// カメラ・プレイヤーからのオフセット
	Vector3 cameraOffset_ = {0.0f, 3.0f, 70.0f};

	// グリッドの描画・表示切り替え用のフラグ
	bool showGrid_ = false;
};
