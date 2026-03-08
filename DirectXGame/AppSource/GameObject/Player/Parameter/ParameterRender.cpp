#include "ParameterRender.h"
#include <numbers>
#include <algorithm>

using namespace SHEngine;

void ParameterRender::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	auto textureManager = engine->GetTextureManager();

	// パラメータのモデルを初期化
	for (int i = 0; i < kParameterCount; ++i) {
		renders_[i] = std::make_unique<RenderObject>();
		renders_[i]->Initialize();

		// シェーダー設定
		renders_[i]->psoConfig_.vs = "Game/Field.VS.hlsl";
		renders_[i]->psoConfig_.ps = "Game/Field.PS.hlsl";
		renders_[i]->SetUseTexture(true);

		// テクスチャの読み込み
		textureIndexes_[i] = textureManager->LoadTexture("PlayerParameter/" + texturePaths[i]);

		// CBVの生成
		renders_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		renders_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renders_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		// 描画データを設定
		renders_[i]->SetDrawData(drawData);

		// ワールド座標を設定
		transforms_[i].position.x = 1000.0f;
		transforms_[i].position.y = i * -margin_ + startPosY_;
		transforms_[i].position.z = 0.0f;

		// 回転
		transforms_[i].rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};

		// スケール
		transforms_[i].scale = {50.0f, 150.0f, 1.0f};

		// 単位行列の代入
		wvps_[i] = Matrix4x4::Identity();
	}

	// 0〜9の数字テクスチャ読み込み
	for (int i = 0; i < 10; ++i) {
		digitTextureIndexes_[i] = textureManager->LoadTexture("Numbers/" + std::to_string(i) + ".png");
	}

	// 数字用モデルの初期化 (各パラメータ3桁分)
	for (int i = 0; i < kParameterCount; ++i) {
		for (int j = 0; j < 3; ++j) {
			digitRenders_[i][j] = std::make_unique<RenderObject>();
			digitRenders_[i][j]->Initialize();
			digitRenders_[i][j]->psoConfig_.vs = "Game/Field.VS.hlsl";
			digitRenders_[i][j]->psoConfig_.ps = "Game/Field.PS.hlsl";
			digitRenders_[i][j]->SetUseTexture(true);

			digitRenders_[i][j]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
			digitRenders_[i][j]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
			digitRenders_[i][j]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

			digitRenders_[i][j]->SetDrawData(drawData);

			// 初期Transformの設定
			digitTransforms_[i][j].scale = digitScale_;
			digitTransforms_[i][j].rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};
			digitTransforms_[i][j].position.z = 0.0f;

			digitWvps_[i][j] = Matrix4x4::Identity();
		}
	}

	// 背景用モデル生成&初期化
	backgroundRender_=std::make_unique<RenderObject>();
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
	backgroundTransform_.position = {1100.0f, 10.0f, 0.0f};
	backgroundTransform_.scale = {400.0f, 2000.0f, 0.0f};

	// wvp
	backgroundWVP_ = Matrix::MakeAffineMatrix(backgroundTransform_.scale, backgroundTransform_.rotate, backgroundTransform_.position);
}

void ParameterRender::Update(Matrix4x4 vpMatrix, const ParameterData& parameterData) {
	// ParameterDataから13個の値を抽出し、最大3桁(0〜999)に制限
	std::array<int, kParameterCount> paramValues = {
	    std::clamp(static_cast<int>(parameterData.maxHP), 0, 999),
	    std::clamp(static_cast<int>(parameterData.hpRegen), 0, 999),
	    std::clamp(static_cast<int>(parameterData.lifeStealPercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.damagePercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.meleeDamage), 0, 999),
	    std::clamp(static_cast<int>(parameterData.rangedDamage), 0, 999),
	    std::clamp(static_cast<int>(parameterData.attackSpeedPercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.criticalRatePercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.dodgeRatePercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.defense), 0, 999),
	    std::clamp(static_cast<int>(parameterData.moveSpeedPercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.expBonusPercent), 0, 999),
	    std::clamp(static_cast<int>(parameterData.moneyBonusPercent), 0, 999)};

	for (int i = 0; i < kParameterCount; ++i) {
		transforms_[i].position.y = i * -margin_ + startPosY_;

		wvps_[i] = Matrix::MakeAffineMatrix(transforms_[i].scale, transforms_[i].rotate, transforms_[i].position);
		wvps_[i] *= vpMatrix;
		renders_[i]->CopyBufferData(0, &wvps_[i], sizeof(Matrix4x4));

		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));

		renders_[i]->CopyBufferData(2, &textureIndexes_[i], sizeof(int));

		// 数値の桁分解と座標計算
		int val = paramValues[i];
		if (val >= 100) {
			activeDigitCounts_[i] = 3;
			currentDigits_[i][0] = val / 100;       // 百の位
			currentDigits_[i][1] = (val / 10) % 10; // 十の位
			currentDigits_[i][2] = val % 10;        // 一の位
		} else if (val >= 10) {
			activeDigitCounts_[i] = 2;
			currentDigits_[i][0] = val / 10; // 十の位
			currentDigits_[i][1] = val % 10; // 一の位
		} else {
			activeDigitCounts_[i] = 1;
			currentDigits_[i][0] = val; // 一の位
		}

		for (int j = 0; j < activeDigitCounts_[i]; ++j) {
			// Y座標はラベルと同じ
			digitTransforms_[i][j].position.y = transforms_[i].position.y;

			// X座標はラベルからoffsetX分ずらし、桁ごとにmarginX分ずらす(左から右へ)
			digitTransforms_[i][j].position.x = transforms_[i].position.x + digitOffsetX_ + (j * digitMarginX_);
			digitTransforms_[i][j].scale = digitScale_;

			digitWvps_[i][j] = Matrix::MakeAffineMatrix(digitTransforms_[i][j].scale, digitTransforms_[i][j].rotate, digitTransforms_[i][j].position);
			digitWvps_[i][j] *= vpMatrix;

			digitRenders_[i][j]->CopyBufferData(0, &digitWvps_[i][j], sizeof(Matrix4x4));
			digitRenders_[i][j]->CopyBufferData(1, &color, sizeof(Vector4));

			// 分解した数値に対応するテクスチャインデックスを渡す
			int texIndex = digitTextureIndexes_[currentDigits_[i][j]];
			digitRenders_[i][j]->CopyBufferData(2, &texIndex, sizeof(int));
		}
	}

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
		renders_[i]->Draw(cmdObj);

		// 有効な桁数分だけ数字を描画
		for (int j = 0; j < activeDigitCounts_[i]; ++j) {
			digitRenders_[i][j]->Draw(cmdObj);
		}
	}

#ifdef USE_IMGUI
	ImGui::Begin("ParameterRender");
	ImGui::DragFloat("StartPosY", &startPosY_, 0.01f);
	ImGui::DragFloat("Margin", &margin_, 0.01f);
	for (int i = 0; i < kParameterCount; ++i) {
		std::string scaleLabel = "Scale" + std::to_string(i);
		std::string rotateLabel = "Rotate" + std::to_string(i);
		std::string positionLabel = "Position" + std::to_string(i);
		ImGui::DragFloat3(scaleLabel.c_str(), &transforms_[i].scale.x, 0.01f);
		ImGui::DragFloat3(rotateLabel.c_str(), &transforms_[i].rotate.x, 0.01f);
		ImGui::DragFloat3(positionLabel.c_str(), &transforms_[i].position.x, 0.01f);
	}
	ImGui::End();
#endif
}