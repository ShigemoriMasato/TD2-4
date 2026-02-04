#include "Difficult.h"

void Difficult::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, FontLoader* fontLoader, bool reset) {
	if (reset) {
		difficult_ = 0;
	}

	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	NodeModelData modelData = modelManager_->GetNodeModelData(modelManager->LoadModel("Needle"));
	DrawData drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	triangle_ = std::make_unique<RenderObject>();
	triangle_->Initialize();
	triangle_->SetDrawData(drawData);
	triangle_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "wvpMatrix");
	triangle_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "color");
	triangle_->psoConfig_.vs = "Simple.VS.hlsl";
	triangle_->psoConfig_.ps = "Color.PS.hlsl";

	modelData = modelManager->GetNodeModelData(modelManager->LoadModel("Meter"));
	drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	meter_ = std::make_unique<RenderObject>();
	meter_->Initialize();
	meter_->SetDrawData(drawData);
	meter_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "wvpMatrix");
	meter_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "textureIndex");
	meter_->psoConfig_.vs = "Simple.VS.hlsl";
	meter_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	meter_->psoConfig_.rootConfig.samplers = uint32_t(SamplerID::ClampClamp_MinMagNearest);
	meter_->SetUseTexture(true);
	meterTextureIndex_ = modelData.materials[modelData.materialIndex[0]].textureIndex;

	difficultText_ = std::make_unique<FontObject>();
	drawData = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);
	difficultText_->Initialize("YDWbananaslipplus.otf", L"衝突時 Oreの体力減少", drawData, fontLoader);

	difficultNumber_ = std::make_unique<Number>();
	difficultNumber_->Initialize("YDWbananaslipplus.otf", drawData, fontLoader);

	difficultNumber_->SetNumber(difficult_ * 15);
	difficultNumber_->SetAfterOffset(L" %");

	number = difficult_ * 15;
}

void Difficult::Update(float deltaTime) {
	timer_ += deltaTime;
	//揺れ処理
	hovered_.y = sinf(timer_ * 2.0f) * 0.1f;
	t = std::min(t + deltaTime * 0.8f, 1.0f);

	//三角形が下がる処理
	position_.y = lerp(position_.y, targetPosition_.y, t, EaseType::EaseOutBack);
	position_.x = lerp(position_.x, targetPosition_.x, t, EaseType::EaseInOutCirc);

	posY_ = lerp(prevY_, targetY_, t, EaseType::EaseOutExpo);

	modelParentTransform_.rotate.y = lerp(targetRotateY_, 0.0f, t, EaseType::EaseOutExpo);

	number = lerp(number, difficult_ * 15, t);

	difficultNumber_->SetNumber(number);
	difficultNumber_->Update(deltaTime);
}

void Difficult::Draw(Window* window, const Matrix4x4& vpMat) {

	Matrix4x4 allParentMatrix = Matrix::MakeTranslationMatrix({ 0.0f, 0.0f, 20.0f }) * Matrix::MakeRotationYMatrix(3.14159265358f);

	Matrix4x4 modelParentMatrix = Matrix::MakeAffineMatrix(modelParentTransform_.scale, modelParentTransform_.rotate, modelParentTransform_.position + hovered_) * allParentMatrix;

	//三角形の描画
	{
		Matrix4x4 triangleMat = Matrix::MakeAffineMatrix({ 1.3f, 1.3f, 1.3f }, { 0.0f, 0.0f, 0.0f }, position_ + Vector3(0.0f, posY_, 0.0f));
		Matrix4x4 wvpMat = triangleMat * modelParentMatrix * vpMat;
		Vector4 color = { 0.1f, 0.6f, 0.2f, 1.0f };
		triangle_->CopyBufferData(0, &wvpMat, sizeof(Matrix4x4));
		triangle_->CopyBufferData(1, &color, sizeof(Vector4));
		triangle_->Draw(window);
	}

	//メーターの描画
	{
		Matrix4x4 meterMat = Matrix::MakeAffineMatrix({ 1.3f, 1.3f, 1.3f }, { 0.0f, 0.0f, 0.0f }, position_);
		Matrix4x4 wvpMat = meterMat * modelParentMatrix * vpMat;
		int textureIndex = difficult_;
		meter_->CopyBufferData(0, &wvpMat, sizeof(Matrix4x4));
		meter_->CopyBufferData(1, &meterTextureIndex_, sizeof(int));
		meter_->Draw(window);
	}

	if (uiDraw_)
		//説明文の描画
	{
		Camera camera;
		camera.SetProjectionMatrix(OrthographicDesc());
		camera.MakeMatrix();

		Matrix4x4 vpMatrix = camera.GetVPMatrix();

		float alpha = std::min(timer_ - 1.5f, 1.0f);
		Vector4 color = { 1.0f, 1.0f, 1.0f, alpha };
		difficultText_->transform_.position = { 500, 400, 0.0f };
		difficultText_->fontColor_ = color;
		difficultText_->Draw(window, vpMatrix);

		difficultNumber_->SetColor(ConvertColor({ 0.5f, 0.5f, 0.5f, alpha }), ConvertColor({ 1.0f, 1.0f, 1.0f, alpha }));
		difficultNumber_->transform_.scale = { 1.5f, -1.5f, 1.5f };
		difficultNumber_->transform_.position = { 900, 500, 0.0f };
		difficultNumber_->Draw(window, vpMatrix);
	}

}

void Difficult::DifficultyUp(bool up) {
	if (up) {
		difficult_++;

		prevY_ = targetY_;
		targetY_ = -downHeight_ * difficult_;
	}

	t = 0;
	position_ = targetPosition_;
	targetPosition_ = { -6.0f, 0.0f, 0.0f };

	targetRotateY_ = 3.1415926535f * 6;

	uiDraw_ = true;

	timer_ = 0.0f;
}
