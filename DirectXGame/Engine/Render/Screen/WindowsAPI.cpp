#include "WindowsAPI.h"
#include <unordered_map>
#include <Utility/ConvertString.h>

using namespace SHEngine;

namespace {
	std::unordered_map<HWND, WindowsAPI*> windowMap;	///< ウィンドウハンドルとWindowsAPIインスタンスのマップ
}

SHEngine::WindowsAPI::~WindowsAPI() {
	// ウィンドウの破棄
	if (hwnd_) {
		windowMap.erase(hwnd_);
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}
}

void SHEngine::WindowsAPI::Initialize(WindowDesc& desc, HINSTANCE hInstance) {
	logger_ = getLogger("Engine");

	std::string logMsg = "Initializing WindowsAPI with width: " + std::to_string(desc.width) +
		", height: " + std::to_string(desc.height) +
		", windowName: " + ConvertString(desc.windowName);

	logger_->info(logMsg);

	// ウィンドウクラスの登録
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = desc.windowName.c_str();
	wc_.hInstance = hInstance;
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc_);

	RECT wrc = { 0, 0, desc.width, desc.height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの作成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,
		wc_.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wrc.right - wrc.left, wrc.bottom - wrc.top,
		nullptr, nullptr, wc_.hInstance, nullptr
	);

	if (!hwnd_) {
		logger_->error("Failed to create window.");
		return;
	}

	// ウィンドウハンドルとWindowsAPIインスタンスのマップに登録
	windowMap[hwnd_] = this;

	ShowWindow(hwnd_, SW_SHOW);
}

LRESULT CALLBACK SHEngine::WindowsAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	auto it = windowMap.find(hwnd);
	if (it != windowMap.end()) {

		if (msg == WM_DESTROY) {
			it->second->logger_->info("Window is pushed Close Button.");
			it->second->isPushCloseButton_ = true;
			return 0;
		}

		//WndProcが登録されていない場合はDefWindowProcを呼び出す
		if (!it->second->desc_.wndProc) {
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return it->second->desc_.wndProc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
