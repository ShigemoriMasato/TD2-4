#pragma once
#include "Display/SwapChain.h"
#include "Display/WindowsApp.h"
#include <Core/CmdListManager.h>

/**
 * @struct WindowConfig
 * @brief ウィンドウの設定を保持する構造体
 */
struct WindowConfig {
/// @brief ウィンドウ名
std::wstring windowName = L"SHEngine";
/// @brief ウィンドウクラス名
std::wstring windowClassName = L"SHEngineClass";
/// @brief クライアント領域の幅
int32_t clientWidth = 1280;
/// @brief クライアント領域の高さ
int32_t clientHeight = 720;

/// @brief ウィンドウプロシージャ関数
std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
		};
};

/**
 * @class Window
 * @brief ウィンドウとその描画状態を管理するクラス
 * 
 * WindowsAPIウィンドウ、スワップチェーン、コマンドオブジェクトを統合管理し、
 * 描画前処理、描画後処理、Present処理を提供する。
 */
class Window {
public:

	/// @brief デフォルトコンストラクタ
	Window() = default;
	/// @brief デストラクタ
	~Window() = default;

	/**
	 * @brief ウィンドウを初期化する
	 * @param device DirectX12デバイス
	 * @param textureManager テクスチャマネージャー
	 * @param cmdListManager コマンドリストマネージャー
	 * @param config ウィンドウ設定
	 * @param clearColor クリアカラー
	 */
	void Initialize(DXDevice* device, TextureManager* textureManager, CmdListManager* cmdListManager, const WindowConfig& config, uint32_t clearColor);

	/**
	 * @brief 描画可能状態にする
	 * @param isClear 画面をクリアするかどうか
	 */
	void PreDraw(bool isClear);
	/**
	 * @brief テクスチャとして使用可能な状態にする
	 */
	void ToTexture();
	/**
	 * @brief Presentできる状態にする
	 */
	void PostDraw();

	/**
	 * @brief Presentを実行する（エンジン側で呼ぶ）
	 */
	void Present();

	/// @brief ウィンドウハンドルを取得
	/// @return HWND
	HWND GetHwnd() const { return windowApp_->GetHwnd(); }
	/// @brief コマンドオブジェクトを取得
	/// @return CommandObject*
	CommandObject* GetCommandObject() { return  cmdObject_.get(); }
	/// @brief 現在のディスプレイを取得
	/// @return Display*
	Display* GetDisplay() const { return swapChain_->GetCurrentDisplay(); }

private:

	/// @brief コマンドオブジェクト
	std::shared_ptr<CommandObject> cmdObject_ = nullptr;

	/// @brief WindowsAPIウィンドウ
	std::unique_ptr<WindowsApp> windowApp_ = nullptr;
	/// @brief スワップチェーン
	std::unique_ptr<SwapChain> swapChain_ = nullptr;

	/// @brief エンジン標準のSRVマネージャー
	SRVManager* srvManager_ = nullptr;

	/// @brief PostDrawが呼ばれたかどうか
	bool isPostDraw_ = false;

};
