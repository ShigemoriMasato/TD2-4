#pragma once
#include <Scene/IScene.h>
#include <Render/RenderObject.h>
#include <Camera/DebugCamera.h>
#include <Render/DebugLine.h>

#include"Common/DebugParam/ParamManager.h"

class TestScene : public IScene {
public:

	TestScene() = default;
	~TestScene() = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_;

	struct VSData {
		Matrix4x4 worldMatrix = Matrix4x4::Identity();
		Matrix4x4 vpMatrix = Matrix4x4::Identity();
	};
	std::vector<WellForGPU> skinningMatrices_;
	std::unique_ptr<RenderObject> renderObject_;
	VSData vsData_{};
	int vsDataIndex_ = -1;
	std::unique_ptr<DebugLine> debugLine_;

	std::unique_ptr<RenderObject> fontTest_;
	Matrix4x4 wvpMat_{};
	std::vector<CharPosition> charPositions_;
	Vector4 fontColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
	char imguiBuffer_[256] = "ABC";
	std::wstring text_ = L"ABC";

	std::string fontName = "ZenOldMincho-Medium.ttf";

	// パラメーター管理
	std::unique_ptr<ParamManager> paramManager_;
	// テストパラメーター
	float testParam_ = 0.0f;
};
