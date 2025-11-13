#include "SHEngine.h"

SHEngine::SHEngine() {
	dxDevice_ = std::make_unique<DXDevice>();
}

SHEngine::~SHEngine() {
}

void SHEngine::Initialize() {
	dxDevice_->Initialize();
}

bool SHEngine::IsLoop() {
	while (msg_.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg_);
			DispatchMessage(&msg_);
		} else {
			//メッセージがなければ処理を始める
			return true;
		}
	}

	return false;
}

void SHEngine::Update() {
}
