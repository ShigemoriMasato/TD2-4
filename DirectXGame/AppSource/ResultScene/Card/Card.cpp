#include "Card.h"
#include <Utility/Easing.h>

void Card::Initialize(const std::string& fontName, const DrawData& drawData, FontLoader* fontLoader, TextureManager* textureManager, int floorNum, bool isFalse) {
	// カード枚数表示の初期化
	number_.resize(2);
	for (int i = 0; i < 2; i++) {
		number_[i] = std::make_unique<Number>();
		number_[i]->Initialize(fontName, drawData, fontLoader);
		number_[i]->SetColor(0xffffffff, 0xffff0000);
		number_[i]->SetMaxScale(1.5f);
		number_[i]->LeftAlign(i);
	}
	// スラッシュの初期化
	slash_ = std::make_unique<FontObject>();
	slash_->Initialize(fontName, L"/", drawData, fontLoader);
	slash_->SetText(L"/");
	slash_->fontColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	// カードの背景の初期化
	cardBack_ = std::make_unique<RenderObject>();
	cardBack_->Initialize();
	cardBack_->SetDrawData(drawData);
	cardBack_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	cardBack_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "Texture");
	cardBack_->SetUseTexture(true);
	cardBack_->psoConfig_.vs = "Simple.VS.hlsl";
	cardBack_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	cardBack_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	cbTextureIndex_ = textureManager->LoadTexture("ScoreCard.png");
	// フロア数の初期化
	floorNum_ = std::make_unique<FontObject>();
	floorNum_->Initialize(fontName, L"F" + std::to_wstring(floorNum), drawData, fontLoader);
	
	// スタンプの初期化
	stamp_ = std::make_unique<RenderObject>();
	stamp_->Initialize();
	stamp_->SetDrawData(drawData);
	stamp_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	stamp_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "Texture");
	stamp_->SetUseTexture(true);
	stamp_->psoConfig_.vs = "Simple.VS.hlsl";
	stamp_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	spTextureIndex_ = textureManager->LoadTexture((isFalse ? "NG" : "OK") + std::string("_Mark.png"));

	isFalse_ = isFalse;

	Load();
}

bool Card::Update(float deltaTime, const Matrix4x4& parentMatrix) {
	transform_.position = lerp(transform_.position, targetTransform_.position, deltaTime * 10.0f);
	transform_.scale = lerp(transform_.scale, targetTransform_.scale, deltaTime * 10.0f);

	timer_ += deltaTime;
	
	transform_.scale = lerp(Vector3(2.0f, 2.0f, 2.0f), Vector3(1.0f, 1.0f, 1.0f), std::min(timer_ * 2.0f, 1.0f), EaseType::EaseOutInBack);

	if (isFalse_) {
		float t = std::clamp((timer_ - 1.0f) * 5.0f, 0.0f, 1.0f);
		transform_.rotate.z = lerp(0.0f, 0.3f, t, EaseType::EaseInCubic);
	}

	//子 x 親
	parentMatrix_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position) * parentMatrix;

	for (int i = 0; i < 2; ++i) {
		number_[i]->Update(deltaTime);
	}

	return timer_ > 3.0f;
}

void Card::Draw(Window* window, const Matrix4x4& vpMatrix) {
	//背景
	Matrix4x4 cbMat = Matrix::MakeAffineMatrix(cbTransform_.scale, cbTransform_.rotate, cbTransform_.position) * parentMatrix_ * vpMatrix;
	cbMat.m[2][2] = 1.0f; //Z軸固定
	cardBack_->CopyBufferData(0, &cbMat, sizeof(Matrix4x4));
	cardBack_->CopyBufferData(1, &cbTextureIndex_, sizeof(int));
	cardBack_->Draw(window);

	//文字系
	Matrix4x4 stringMat = parentMatrix_ * vpMatrix;
	slash_->Draw(window, stringMat);
	floorNum_->Draw(window, stringMat);

	//数字系
	for (int i = 0; i < 2; ++i) {
		number_[i]->Draw(window, stringMat);
	}

	//スタンプ
	Matrix4x4 spMat = Matrix::MakeAffineMatrix(spTransform_.scale, spTransform_.rotate, spTransform_.position) * parentMatrix_ * vpMatrix;
	stamp_->CopyBufferData(0, &spMat, sizeof(Matrix4x4));
	stamp_->CopyBufferData(1, &spTextureIndex_, sizeof(int));
	stamp_->Draw(window);
}

void Card::DrawImGui() {

#ifdef USE_IMGUI

	ImGui::Begin("Card");

	ImGui::DragFloat3("CB Scale", &cbTransform_.scale.x, 1.f);
	ImGui::DragFloat3("CB Rotate", &cbTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("CB Position", &cbTransform_.position.x, 1.f);
	ImGui::Separator();
	ImGui::DragFloat3("SP Scale", &spTransform_.scale.x, 1.f);
	ImGui::DragFloat3("SP Rotate", &spTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("SP Position", &spTransform_.position.x, 1.f);
	ImGui::Separator();
	ImGui::DragFloat3("Slash Scale", &slash_->transform_.scale.x, 1.f);
	ImGui::DragFloat3("Slash Rotate", &slash_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Slash Position", &slash_->transform_.position.x, 1.f);
	ImGui::Separator();
	for (int i = 0; i < 2; ++i) {
		std::string label = "Number " + std::to_string(i);
		ImGui::DragFloat3(("Num" + std::to_string(i) + " Scale").c_str(), &number_[i]->transform_.scale.x, 0.01f);
		ImGui::DragFloat3(("Num" + std::to_string(i) + " Rotate").c_str(), &number_[i]->transform_.rotate.x, 0.01f);
		ImGui::DragFloat3(("Num" + std::to_string(i) + " Position").c_str(), &number_[i]->transform_.position.x, 1.f);
		ImGui::Separator();
	}
	ImGui::DragFloat3("FloorNum Scale", &floorNum_->transform_.scale.x, 1.f);
	ImGui::DragFloat3("FloorNum Rotate", &floorNum_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("FloorNum Position", &floorNum_->transform_.position.x, 1.f);

	ImGui::End();

#endif

}

void Card::SetTransData(const Vector3& position, float scale) {
	targetTransform_.position = position;
	targetTransform_.scale = Vector3(scale, scale, scale);
}

void Card::Load() {
	auto values = bManager_.Read("EndCard");
	if(values.empty()) {
		return;
	}
	int index = 0;
	cbTransform_.scale = bManager_.Reverse<Vector3>(values[index++].get());
	cbTransform_.rotate = bManager_.Reverse<Vector3>(values[index++].get());
	cbTransform_.position = bManager_.Reverse<Vector3>(values[index++].get());
	spTransform_.scale = bManager_.Reverse<Vector3>(values[index++].get());
	spTransform_.rotate = bManager_.Reverse<Vector3>(values[index++].get());
	spTransform_.position = bManager_.Reverse<Vector3>(values[index++].get());
	slash_->transform_.scale = bManager_.Reverse<Vector3>(values[index++].get());
	slash_->transform_.rotate = bManager_.Reverse<Vector3>(values[index++].get());
	slash_->transform_.position = bManager_.Reverse<Vector3>(values[index++].get());
	for (int i = 0; i < 2; ++i) {
		number_[i]->transform_.scale = bManager_.Reverse<Vector3>(values[index++].get());
		number_[i]->transform_.rotate = bManager_.Reverse<Vector3>(values[index++].get());
		number_[i]->transform_.position = bManager_.Reverse<Vector3>(values[index++].get());
	}
	floorNum_->transform_.scale = bManager_.Reverse<Vector3>(values[index++].get());
	floorNum_->transform_.rotate = bManager_.Reverse<Vector3>(values[index++].get());
	floorNum_->transform_.position = bManager_.Reverse<Vector3>(values[index++].get());
}

void Card::Save() {
	bManager_.RegisterOutput(cbTransform_.scale);
	bManager_.RegisterOutput(cbTransform_.rotate);
	bManager_.RegisterOutput(cbTransform_.position);
	bManager_.RegisterOutput(spTransform_.scale);
	bManager_.RegisterOutput(spTransform_.rotate);
	bManager_.RegisterOutput(spTransform_.position);
	bManager_.RegisterOutput(slash_->transform_.scale);
	bManager_.RegisterOutput(slash_->transform_.rotate);
	bManager_.RegisterOutput(slash_->transform_.position);
	for (int i = 0; i < 2; ++i) {
		bManager_.RegisterOutput(number_[i]->transform_.scale);
		bManager_.RegisterOutput(number_[i]->transform_.rotate);
		bManager_.RegisterOutput(number_[i]->transform_.position);
	}
	bManager_.RegisterOutput(floorNum_->transform_.scale);
	bManager_.RegisterOutput(floorNum_->transform_.rotate);
	bManager_.RegisterOutput(floorNum_->transform_.position);
	bManager_.Write("EndCard");
}
