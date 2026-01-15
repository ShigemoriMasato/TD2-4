#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <wrl.h>

#include "Utility/MyMath.h"

/**
 * @enum XBoxController
 * @brief XBoxコントローラーのボタン定義
 */
enum class XBoxController {
	kUnknown = -1,
	kA,
	kB,
	kX,
	kY,
	kUp,
	kDown,
	kLeft,
	kRight,
	kSelect,
	kStart,
	kRightShoulder,
	kRightTrigger,
	kLeftShoulder,
	kLeftTrigger,

	kButtomCount
};

/**
 * @class Input
 * @brief キーボード、マウス、XBoxコントローラーの入力を管理するクラス
 * 
 * DirectInputとXInputを使用して各種入力デバイスの状態を取得・管理する。
 * 現在フレームと前フレームの入力状態を保持し、入力判定を可能にする。
 */
class Input {
public:
	/// @brief デフォルトコンストラクタ
	Input() {}
	/// @brief デストラクタ
	~Input() = default;

	/**
	 * @brief ウィンドウハンドルを設定
	 * @param hwnd ウィンドウハンドル
	 */
	void SetHWND(HWND hwnd) {
		hwnd_ = hwnd;
	}

	/**
	 * @brief ウィンドウを設定
	 * @param hwnd ウィンドウハンドル
	 */
	void SetWindow(HWND hwnd);

	/**
	 * @brief 入力状態を更新する
	 */
	void Update();

	/// @brief 現在のキーボード状態を取得
	/// @return キー状態の配列
	static BYTE* GetKeyState();

	/// @brief 前フレームのキーボード状態を取得
	/// @return キー状態の配列
	static BYTE* GetPreKeyState();

	/// @brief 指定キーの現在の状態を取得
	/// @param keyCode キーコード
	/// @return キーの状態
	static BYTE GetKeyState(int keyCode);

	/// @brief 指定キーの前フレームの状態を取得
	/// @param keyCode キーコード
	/// @return キーの状態
	static BYTE GetPreKeyState(int keyCode);

	/// @brief マウスの移動量を取得
	/// @return マウス移動量
	static Vector2 GetMouseMove();

	/// @brief マウスホイールの回転量を取得
	/// @return ホイール回転量
	static float GetMouseWheel();

	/// @brief マウスボタンの現在の状態を取得
	/// @return ボタン状態の配列
	static BYTE* GetMouseButtonState();

	/// @brief マウスボタンの前フレームの状態を取得
	/// @return ボタン状態の配列
	static BYTE* GetPreMouseButtonState();

	/// @brief XBoxコントローラーのボタン状態を取得
	/// @return ボタン状態の配列
	static bool* GetXBoxButtonState();

	/// @brief XBoxコントローラーの指定ボタンの状態を取得
	/// @param button ボタン種別
	/// @return ボタンが押されているかどうか
	static bool GetXBoxButtonState(XBoxController button);

	/**
	 * @brief XBoxコントローラーのスティック状態を取得
	 * @param type 0:右スティック 1:左スティック
	 * @return スティックの傾き
	 */
	static Vector2 GetXBoxStickState(int type);

private:

/// @brief コントローラー入力状態をクリア
void ControllerClear();
/// @brief マウス・キーボード入力状態をクリア
void MouseKeyboardClear();

friend class SHEngine;
/**
* @brief 入力システムを初期化
* @param hInstance アプリケーションインスタンスハンドル
*/
void Initialize(HINSTANCE hInstance);

	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	
	XINPUT_STATE xBoxState_{};
	/// @brief XBoxボタン状態フラグ
	static bool xBoxButtonFlug_[int(XBoxController::kButtomCount)];
	/// @brief XBoxスティック状態（0:右 1:左）
	static Vector2 xBoxStickState_[2];

	/// @brief 現在のキー状態（staticでどこからでもアクセス可能）
	static BYTE keyState[256];
	/// @brief 前フレームのキー状態
	static BYTE preKeyState[256];

	/// @brief 前フレームのマウス状態
	static DIMOUSESTATE preMouseState;
	/// @brief 現在のマウス状態
	static DIMOUSESTATE mouseState;

	/// @brief 初期化済みフラグ（static関数使用の為に必要）
	static bool isInitialized_;

	/// @brief アプリケーションインスタンスハンドル
	HINSTANCE hInstance_ = nullptr;
	/// @brief ウィンドウハンドル
	HWND hwnd_ = nullptr;

};
