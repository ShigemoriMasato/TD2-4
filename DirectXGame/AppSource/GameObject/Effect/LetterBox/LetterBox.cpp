#include "LetterBox.h"
#include <Utility/Matrix.h>

void LetterBox::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	SHEngine::DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	render_ = std::make_unique<SHEngine::RenderObject>("LetterBox");
	render_->Initialize();
	render_->SetDrawData(data);
	// SituationGaugeの武器アイコンなどを参考に、テクスチャ+カラーのシェーダーを指定
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "TexColor.PS.hlsl";
	// インスタンシングで上と下の2つを描画
	render_->CreateSRV(sizeof(Matrix4x4), 2, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);
	render_->instanceNum_ = 2;

	// 初期設定：Xスケールは画面幅、Yスケールは帯の高さ
	transformTop_.scale = {screenWidth_, boxHeight_, 1.0f};
	transformTop_.rotate = {0.0f, 0.0f, 0.0f};

	transformBottom_.scale = {screenWidth_, boxHeight_, 1.0f};
	transformBottom_.rotate = {0.0f, 0.0f, 0.0f};

	// イージングの初期化 (開始:0.0, 終了:1.0)
	easing_ = EasingAnimation<float>(0.0f, 1.0f, 1.0f, EaseType::EaseOutCubic);
}

void LetterBox::Update(Matrix4x4 vpMatrix, float deltaTime) {
	if (!isTriggered_)
		return;

	// イージングを更新して進行度 (progress_) を取得
	easing_.Update(deltaTime, progress_);

	// 進行度に応じてY座標を補間 (ここでは画面の原点やカメラに合わせて適宜調整してください)
	// 例: 上の帯は上端の画面外から画面内へ、下の帯は下端の画面外から画面内へ
	// ※ Y軸が下向き(Orthographic)の想定で記述しています。
	float topStartY = boxHeight_ * 0.5f;
	float topEndY = -boxHeight_ * 0.5f;
	transformTop_.position = {screenWidth_ * 0.5f, std::lerp(topStartY, topEndY, progress_), 0.0f};

	float bottomStartY = -screenHeight_ - boxHeight_ * 0.5f;
	float bottomEndY = -screenHeight_ + boxHeight_ * 0.5f;
	transformBottom_.position = {screenWidth_ * 0.5f, std::lerp(bottomStartY, bottomEndY, progress_), 0.0f};

	// 描画データの転送
	Matrix4x4 wvpMatrices[2];
	Matrix4x4 topWorld = Matrix::MakeAffineMatrix(transformTop_.scale, transformTop_.rotate, transformTop_.position);
	wvpMatrices[0] = topWorld * vpMatrix;

	Matrix4x4 bottomWorld = Matrix::MakeAffineMatrix(transformBottom_.scale, transformBottom_.rotate, transformBottom_.position);
	wvpMatrices[1] = bottomWorld * vpMatrix;

	Vector4 color = {0.0f, 0.0f, 0.0f, 1.0f}; // 黒色
	int textureIndex = 0;                     // 単色表示なので、適当な白画像等のインデックスを指定

	render_->CopyBufferData(0, wvpMatrices, sizeof(Matrix4x4) * 2);
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex, sizeof(int));
}

void LetterBox::Draw(CmdObj* cmdObj) {
	if (!isTriggered_)
		return;
	render_->Draw(cmdObj);
}

void LetterBox::Trigger() {
	if (!isTriggered_) {
		isTriggered_ = true;
		// 1.5秒かけてEaseOutQuartでアニメーション開始
		easing_.Start(0.0f, 1.0f, 1.5f, EaseType::EaseOutQuart);
	}
}