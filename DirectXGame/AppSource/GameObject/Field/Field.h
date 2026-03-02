#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <array>

class Field {
public:
	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新関数
	void Update(Matrix4x4 vpMatrix);

	// 描画関数
	void Draw(CmdObj* cmdObj);

private:
	// フィールドモデルの数
	static constexpr int kFieldModelCount = 4;

	// グリッドサイズ
	static constexpr int kGridWidth = 2;
	static constexpr int kGridHeight = 2;

	// モデル間隔
	static constexpr float kModelSpacing = 20.0f;

	// テクスチャのインデックス
	int textureIndex_;

	// 描画用変数
	std::array<std::unique_ptr<SHEngine::RenderObject>, kFieldModelCount> renders_ = {};

	// WVP行列
	std::array<Matrix4x4, kFieldModelCount> wvps_ = {};

	// トランスフォーム
	std::array<Transform, kFieldModelCount> transforms_ = {};
};
