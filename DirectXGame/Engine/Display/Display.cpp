#include "Display.h"
#include <unordered_map>
#include <Engine/Logger/Logger.h>

namespace {
	std::unordered_map<HWND, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> windowProcMap;
}

Display::~Display() {
}

void Display::SetWindowProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> windowProc) {
	// ウィンドウハンドルとウィンドウプロシージャをマップに登録する
	windowProc_ = windowProc;
}

void Display::SetSize(int32_t width, int32_t height) {
	clientWidth_ = width;
	clientHeight_ = height;
}

void Display::SetWindowName(std::wstring name) {
	windowName_ = name;
}

void Display::SetWindowClassName(std::wstring name) {
	windowClassName_ = name;
}

void Display::Create() {
	auto logger = Logger::getLogger("Window");

    //ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    //ウィンドウクラス名
    wc_.lpszClassName = windowClassName_.c_str();
    //インスタンスハンドル
    wc_.hInstance = GetModuleHandleA(nullptr);
    //カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //ウィンドウクラスの登録
    RegisterClass(&wc_);

    //ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc = { 0, 0, clientWidth_, clientHeight_ };

    //クライアント領域をもとに実際のサイズにwrcに変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    //ウィンドウの作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,			//利用するクラスの名前
        windowName_.c_str(),			//タイトルバーの文字
        WS_OVERLAPPEDWINDOW,		//よく見るウィンドウスタイル
        CW_USEDEFAULT,				//表示x座標
        CW_USEDEFAULT,				//表示y座標
        wrc.right - wrc.left,		//ウィンドウ幅
        wrc.bottom - wrc.top,		//ウィンドウ高さ
        nullptr,					//親ウィンドウハンドル
        nullptr,					//メニューハンドル
        wc_.hInstance,				//インスタンスハンドル
        nullptr);					//オプション

	// ウィンドウプロシージャをマップに登録する
	windowProcMap[hwnd_] = windowProc_;
}

void Display::Show(ShowType cmd) {
	ShowWindow(hwnd_, cmd);
}

void Display::Close() {
	CloseWindow(hwnd_);
}

inline void Display::Destroy() {
	DestroyWindow(hwnd_);
}

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// 登録されたウィンドウプロシージャを呼び出す
	auto it = windowProcMap.find(hwnd);
	if(it != windowProcMap.end()) {
		return it->second(hwnd, msg, wparam, lparam);
	}

	//標準のメッセージ
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
