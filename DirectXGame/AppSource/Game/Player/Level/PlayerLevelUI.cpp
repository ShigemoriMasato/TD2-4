#include "PlayerLevelUI.h"

using namespace SHEngine;
using namespace Player;

void LevelUI::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input) {
	// HPバーの生成
	expGaugeFill_.render = std::make_unique<RenderObject>(); // 前面
	expGaugeBG_.render = std::make_unique<RenderObject>();   // 背景

	// HPバーを初期化
	InitializeRenderObj(modelManager, drawDataManager, expGaugeFill_.render); // 前面
	InitializeRenderObj(modelManager, drawDataManager, expGaugeBG_.render);   // 背景

	// 単位行列の代入
	expGaugeFill_.wvp = Matrix4x4::Identity();
	expGaugeBG_.wvp = Matrix4x4::Identity();

	// 前面
	expGaugeFill_.transform.scale = {gaugeWidth_, 1.0f, 1.0f};
	expGaugeFill_.transform.rotate = {0.3f, 0.0f, 0.0f};
	expGaugeFill_.transform.position = {gaugePosX, 6.0f, 0.0f};

	Vector4 color = {1.0f, 1.0f, 0.0f, 1.0f};

	expGaugeFill_.render->CopyBufferData(1, &color, sizeof(Vector4));

	// 背景
	expGaugeBG_.transform.scale = {gaugeWidth_, 1.0f, 1.0f};
	expGaugeBG_.transform.rotate = {0.3f, 0.0f, 0.0f};
	expGaugeBG_.transform.position = {gaugePosX, 6.0f, 0.0f};

	color = {0.0f, 0.0f, 0.0f, 1.0f};

	expGaugeBG_.render->CopyBufferData(1, &color, sizeof(Vector4));

	// FPSObserver
	fpsObserver_ = std::make_unique<FPSObserver>();

	// 入力
	input_ = input;

	// モデルマネージャー
	modelManager_ = modelManager;
}

void LevelUI::Update(Matrix4x4 vpMatrix, float deltaTime, float currentEXP, float maxEXP) {
	// ゲージのスケール変更
	EXPGaugeScaleChange(currentEXP, maxEXP);

	// 前面の行列計算
	expGaugeFill_.wvp = Matrix::MakeAffineMatrix(expGaugeFill_.transform.scale, expGaugeFill_.transform.rotate, expGaugeFill_.transform.position);
	expGaugeFill_.wvp *= vpMatrix;
	expGaugeFill_.render->CopyBufferData(0, &expGaugeFill_.wvp, sizeof(Matrix4x4));

	Vector4 color = {1.0f, 1.0f, 0.0f, 1.0f}; // 黄色

	expGaugeFill_.render->CopyBufferData(1, &color, sizeof(Vector4));

	// 背景の行列計算
	expGaugeBG_.wvp = Matrix::MakeAffineMatrix(expGaugeBG_.transform.scale, expGaugeBG_.transform.rotate, expGaugeBG_.transform.position);
	expGaugeBG_.wvp *= vpMatrix;
	expGaugeBG_.render->CopyBufferData(0, &expGaugeBG_.wvp, sizeof(Matrix4x4));

	color = {0.0f, 0.0f, 0.0f, 1.0f}; // 黒

	expGaugeBG_.render->CopyBufferData(1, &color, sizeof(Vector4));
}

void LevelUI::Draw(CmdObj* cmdObj) {
	// 背景から前面の順に描画
	expGaugeBG_.render->Draw(cmdObj);
	expGaugeFill_.render->Draw(cmdObj);
}

void LevelUI::InitializeRenderObj(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render) {
	render->Initialize();
	render->psoConfig_.vs = "Simple.VS.hlsl";
	render->psoConfig_.ps = "Color.PS.hlsl";

	render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");

	// 描画するデータの読み込み
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render->SetDrawData(drawData);
}

void LevelUI::EXPGaugeScaleChange(float currentEXP, float maxEXP) {
	// 現在のEXPの比率
	float expRatio = currentEXP / maxEXP;

	// 新しいスケール
	float newScale = expRatio * gaugeWidth_;

	// 前面のスケール変更
	expGaugeFill_.transform.scale.x = newScale;

	// X位置を調整してHPバーが左詰めに見えるようにする
	float offsetX = (gaugeWidth_ - expGaugeFill_.transform.scale.x) / 2.0f;
	expGaugeFill_.transform.position.x = gaugePosX - offsetX;
}