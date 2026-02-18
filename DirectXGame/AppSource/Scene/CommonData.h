#pragma once
#include <Render/Screen/WindowsAPI.h>
#include <Render/Screen/SwapChain.h>
#include <Render/Screen/MultiDisplay.h>

using WindowSet = std::pair<std::unique_ptr<SHEngine::Screen::WindowsAPI>, std::unique_ptr<SHEngine::Screen::SwapChain>>;

struct CommonData {

	WindowSet mainWindow;	// メインウィンドウとスワップチェーンのセット
	std::unique_ptr<SHEngine::Screen::MultiDisplay> display = nullptr;	// マルチディスプレイの共通データ

	std::unique_ptr<SHEngine::Command::Object> cmdObject = nullptr;	// コマンドオブジェクトの共通データ
};
