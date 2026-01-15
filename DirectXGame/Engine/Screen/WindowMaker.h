#pragma once
#include <Core/DXDevice.h>
#include <Core/CmdListManager.h>
#include <Assets/Texture/TextureManager.h>
#include "Window.h"

class SHEngine;

/**
 * @class WindowMaker
 * @brief ウィンドウの生成と管理を行うクラス
 * 
 * 複数のウィンドウを作成し、それらの描画終了処理やPresent処理を一括管理する。
 * エンジン全体のウィンドウシステムの中核を担う。
 */
class WindowMaker {
public:

	/**
	 * @brief ウィンドウメーカーを初期化する
	 * @param device DirectX12デバイス
	 * @param textureManager テクスチャマネージャー
	 * @param cmdListManager コマンドリストマネージャー
	 */
	void Initialize(DXDevice* device, TextureManager* textureManager, CmdListManager* cmdListManager);

	/**
	 * @brief 新しいウィンドウを作成する
	 * @param config ウィンドウの設定
	 * @param clearColor クリアカラー
	 * @return 作成されたウィンドウのユニークポインタ
	 */
	std::unique_ptr<Window> MakeWindow(const WindowConfig& config, uint32_t clearColor);

private:

	friend class SHEngine;
	
	/**
	 * @brief 全てのウィンドウの描画終了処理を行う
	 */
	void AllPostDraw();
	
	/**
	 * @brief 全てのウィンドウのPresent処理を行う
	 */
	void PresentAllWindows();

private:

	/// @brief DirectX12デバイスへのポインタ
	DXDevice* device_ = nullptr;
	/// @brief テクスチャマネージャーへのポインタ
	TextureManager* textureManager_ = nullptr;
	/// @brief コマンドリストマネージャーへのポインタ
	CmdListManager* cmdListManager_ = nullptr;

	/// @brief 管理中の全ウィンドウのリスト
	std::vector<Window*> windows_{};

};
