#pragma once
#include <Render/Screen/WindowsAPI.h>
#include <Render/Screen/SwapChain.h>
#include <Common/MainDisplay.h>
#include <Common/KeyConfig/KeyManager.h>
#include <Shop/Piece.h>
#include <GameObject/Player/Parameter/ParameterData.h>
#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/Drawer/TrailDrawer.h>
#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

using WindowSet = std::pair<std::unique_ptr<SHEngine::Screen::WindowsAPI>, std::unique_ptr<SHEngine::Screen::SwapChain>>;

struct CommonData {

	WindowSet mainWindow;	// メインウィンドウとスワップチェーンのセット
	std::unique_ptr<MainDisplay> display = nullptr;	// メインディスプレイ

	std::unique_ptr<SHEngine::Command::Object> cmdObject = nullptr;	// コマンドオブジェクトの共通データ

	std::unique_ptr<KeyManager> keyManager;
	int postEffectDrawDataIndex = -1;

	std::vector<Piece*> pieces; //Playerが今持っているピースのデータ

	bool shouldQuit = false; // アプリケーション終了フラグ

	int stageNum = 0;	//現在のステージの番号

	bool isWin = false; // 勝利フラグ

	// トレイル
	TrailPresetDataBank trailPresetDataBank;
	TrailDrawer trailDrawer;

	// パーティクル
	ParticlePresetDataBank particlePresetDataBank;
	ParticleDrawer particleDrawer;
};
