#include "ParameterRender.h"
#include <algorithm>
#include <numbers>
#include <string>

using namespace SHEngine;

void ParameterRender::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	auto textureManager = engine->GetTextureManager();
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);

	for (int i = 0; i < kParameterCount; ++i) {
		// ラベルテキストの初期化
		texts_[i] = std::make_unique<Text>();
		texts_[i]->Initialize(data, "YDWbananaslipplus.otf", 64, "Parameter : " + std::to_string(i));
		texts_[i]->SetText(texturePaths[i]);

		// 数値テキストの初期化
		valueTexts_[i] = std::make_unique<Text>();
		valueTexts_[i]->Initialize(data, "YDWbananaslipplus.otf", 64, "ParameterNum : " + std::to_string(i));
		valueTexts_[i]->SetText(L"0");
	}

	// 背景用モデル生成&初期化
	backgroundRender_ = std::make_unique<RenderObject>("Parameter BG");
	backgroundRender_->Initialize();

	// シェーダー設定
	backgroundRender_->psoConfig_.vs = "Simple.VS.hlsl";
	backgroundRender_->psoConfig_.ps = "Color.PS.hlsl";

	// CBVの生成
	backgroundRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	backgroundRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	// 描画データを設定
	backgroundRender_->SetDrawData(drawData);

	// Transform
	backgroundTransform_.position = {bgPosX_, 10.0f, 0.0f};
	backgroundTransform_.scale = {400.0f, 2000.0f, 0.0f};

	for (int i = 0; i < kParameterCount; ++i) {
		transforms_[i].position.x = 1000.0f;
		transforms_[i].position.y = i * -40.0f - 200.0f;
	}

	// wvp
	backgroundWVP_ = Matrix::MakeAffineMatrix(backgroundTransform_.scale, backgroundTransform_.rotate, backgroundTransform_.position);
}

void ParameterRender::Update(Matrix4x4 vpMatrix, const std::unordered_map<std::string, float>& parameterData, float deltaTime, std::unordered_map<Key, bool> key) {
	if (key[Key::Debug1]) {
		if (!isAnimation_) {
			AnimationStart();
		} else {
			ReturnAnimationStart();
		}
	}

	offsetAnimation_.anim.Update(deltaTime, offsetAnimation_.temp);

	for (int i = 0; i < kParameterCount; ++i) {
		transforms_[i].position.x = posX_ + offsetAnimation_.temp;
		transforms_[i].position.y = i * marginY_ + startPosY_;

		// 数値テキストのTransform（ラベルからオフセット分ずらす）
		valueTransforms_[i].position.x = transforms_[i].position.x + valueOffsetX_;
		valueTransforms_[i].position.y = transforms_[i].position.y;
		valueTransforms_[i].scale = transforms_[i].scale;
		valueTransforms_[i].rotate = transforms_[i].rotate;
	}

	// ParameterDataから13個の値を抽出し、最大3桁(0〜999)に制限
	std::array<int, kParameterCount> paramValues = {std::clamp(static_cast<int>(parameterData.at("MaxHP")), 0, 999),        std::clamp(static_cast<int>(parameterData.at("HPRegen")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("Damage")), 0, 999),       std::clamp(static_cast<int>(parameterData.at("MeleeDamage")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("RangedDamage")), 0, 999), std::clamp(static_cast<int>(parameterData.at("AttackSpeed")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("AttackCount")), 0, 999),  std::clamp(static_cast<int>(parameterData.at("Penetration")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("KnockBack")), 0, 999),    std::clamp(static_cast<int>(parameterData.at("LifeSteal")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("Move")), 0, 999),         std::clamp(static_cast<int>(parameterData.at("Defence")), 0, 999),
	                                                std::clamp(static_cast<int>(parameterData.at("EXP")), 0, 999),          std::clamp(static_cast<int>(parameterData.at("Gold")), 0, 999)};

	for (int i = 0; i < kParameterCount; ++i) {
		// ラベルテキストの更新
		texts_[i]->Update(vpMatrix);
		texts_[i]->SetTransform(transforms_[i]);

		// 数値テキストの更新
		valueTexts_[i]->SetText(std::to_wstring(paramValues[i]));
		valueTexts_[i]->Update(vpMatrix);
		valueTexts_[i]->SetTransform(valueTransforms_[i]);
	}

	backgroundTransform_.position.x = bgPosX_ + offsetAnimation_.temp;

	// 背景のWVP行列を更新
	backgroundWVP_ = Matrix::MakeAffineMatrix(backgroundTransform_.scale, backgroundTransform_.rotate, backgroundTransform_.position);
	backgroundWVP_ *= vpMatrix;
	backgroundRender_->CopyBufferData(0, &backgroundWVP_, sizeof(Matrix4x4));

	Vector4 bgColor = {0.0f, 0.0f, 0.0f, 0.5f};
	backgroundRender_->CopyBufferData(1, &bgColor, sizeof(Vector4));
}

void ParameterRender::Draw(CmdObj* cmdObj) {
	backgroundRender_->Draw(cmdObj);

	for (int i = 0; i < kParameterCount; ++i) {
		texts_[i]->Draw(cmdObj);
		valueTexts_[i]->Draw(cmdObj);
	}

#ifdef USE_IMGUI
	ImGui::Begin("ParameterRender");
	ImGui::DragFloat("StartPosY", &startPosY_, 0.01f);
	ImGui::DragFloat("PosX", &posX_, 0.01f);
	ImGui::DragFloat("MarginY", &marginY_, 0.01f);
	ImGui::DragFloat("ValueOffsetX", &valueOffsetX_, 0.01f);
	ImGui::End();
#endif
}

void ParameterRender::AnimationStart() {
	if (!offsetAnimation_.anim.GetIsActive()) {
		offsetAnimation_.anim.Start(0.0f, endPosX_, 0.5f, EaseType::EaseOutCubic);
		isAnimation_ = true;
	}
}

void ParameterRender::ReturnAnimationStart() {
	if (!offsetAnimation_.anim.GetIsActive()) {
		offsetAnimation_.anim.Start(endPosX_, 0.0f, 0.5f, EaseType::EaseOutCubic);
		isAnimation_ = false;
	}
}
