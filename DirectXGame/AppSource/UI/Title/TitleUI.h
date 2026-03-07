#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <array>
#include <memory>

class TitleUI {
public:
	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新関数
	void Update(Matrix4x4 vpMatrix);

	// 描画関数
	void Draw(CmdObj* cmdObj);

	// ImGui表示関数
	void DrawImGui();

private:
	// UIモデルの数
	static constexpr int kUIModelCount = 4;

	// UIモデルのインデックス
	enum UIIndex {
		Logo = 0,
		Start = 1,
		Option = 2,
		Quit = 3
	};

	// 描画用変数
	std::array<std::unique_ptr<SHEngine::RenderObject>, kUIModelCount> renders_ = {};

	// WVP行列
	std::array<Matrix4x4, kUIModelCount> wvps_ = {};

	// トランスフォーム
	std::array<Transform, kUIModelCount> transforms_ = {};
};
