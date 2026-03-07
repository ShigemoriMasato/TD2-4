#include "TitleUI.h"

using namespace SHEngine;

void TitleUI::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	// 各UIのモデルパス
	const char* modelPaths[kUIModelCount] = {
		"Assets/Model/UI/Title/Logo",
		"Assets/Model/UI/Title/Start",
		"Assets/Model/UI/Title/Option",
		"Assets/Model/UI/Title/Quit"
	};

	// 4つのUIモデルを初期化
	for (int i = 0; i < kUIModelCount; ++i) {
		// それぞれ異なるモデルを読み込む
		int modelHandle = modelManager->LoadModel(modelPaths[i]);
		auto modelData = modelManager->GetNodeModelData(modelHandle);
		auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

		renders_[i] = std::make_unique<RenderObject>();
		renders_[i]->Initialize();

		// シェーダー設定
		renders_[i]->psoConfig_.vs = "Simple.VS.hlsl";
		renders_[i]->psoConfig_.ps = "Color.PS.hlsl";

		// CBVの生成
		renders_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		renders_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

		// 描画データを設定
		renders_[i]->SetDrawData(drawData);

		// 単位行列の代入
		wvps_[i] = Matrix4x4::Identity();
	}

	// Logo の設定
	transforms_[Logo].position = { 0.0f, 1.2f, 10.0f };
	transforms_[Logo].rotate = { 0.0f, 0.0f, 0.0f };
	transforms_[Logo].scale = {2.0f, 2.0f, 1.0f};

	// Start の設定
	transforms_[Start].position = { 0.0f, 0.0f, 10.0f };
	transforms_[Start].rotate = { 0.0f, 0.0f, 0.0f };
	transforms_[Start].scale = { 1.0f, 1.0f, 1.0f };

	// Option の設定
	transforms_[Option].position = { 0.0f, -0.75f, 10.0f };
	transforms_[Option].rotate = { 0.0f, 0.0f, 0.0f };
	transforms_[Option].scale = { 1.0f, 1.0f, 1.0f };

	// Quit の設定
	transforms_[Quit].position = { 0.0f, -1.5f, 10.0f };
	transforms_[Quit].rotate = { 0.0f, 0.0f, 0.0f };
	transforms_[Quit].scale = { 1.0f, 1.0f, 1.0f };
}

void TitleUI::Update(Matrix4x4 vpMatrix) {
	// 4つのUIモデルを更新
	for (int i = 0; i < kUIModelCount; ++i) {
		wvps_[i] = Matrix::MakeAffineMatrix(transforms_[i].scale, transforms_[i].rotate, transforms_[i].position);
		wvps_[i] *= vpMatrix;
		renders_[i]->CopyBufferData(0, &wvps_[i], sizeof(Matrix4x4));

		// 各UIの色設定
		Vector4 color;
		switch (i) {
		case Logo:
			color = {1.0f, 1.0f, 1.0f, 1.0f}; // 白
			break;
		case Start:
			color = {0.0f, 1.0f, 0.0f, 1.0f}; // 緑
			break;
		case Option:
			color = {0.0f, 0.5f, 1.0f, 1.0f}; // 青
			break;
		case Quit:
			color = {1.0f, 0.0f, 0.0f, 1.0f}; // 赤
			break;
		default:
			color = {1.0f, 1.0f, 1.0f, 1.0f};
			break;
		}

		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));
	}
}

void TitleUI::Draw(CmdObj* cmdObj) {
	// 4つのUIモデルを描画
	for (int i = 0; i < kUIModelCount; ++i) {
		renders_[i]->Draw(cmdObj);
	}
}

void TitleUI::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("TitleUI");

	// Logo Transform
	if (ImGui::CollapsingHeader("Logo", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Logo Position", &transforms_[Logo].position.x, 0.01f);
		ImGui::DragFloat3("Logo Rotate", &transforms_[Logo].rotate.x, 0.01f);
		ImGui::DragFloat3("Logo Scale", &transforms_[Logo].scale.x, 0.01f);
	}

	ImGui::Separator();

	// Start Transform
	if (ImGui::CollapsingHeader("Start", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Start Position", &transforms_[Start].position.x, 0.01f);
		ImGui::DragFloat3("Start Rotate", &transforms_[Start].rotate.x, 0.01f);
		ImGui::DragFloat3("Start Scale", &transforms_[Start].scale.x, 0.01f);
	}

	ImGui::Separator();

	// Option Transform
	if (ImGui::CollapsingHeader("Option", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Option Position", &transforms_[Option].position.x, 0.01f);
		ImGui::DragFloat3("Option Rotate", &transforms_[Option].rotate.x, 0.01f);
		ImGui::DragFloat3("Option Scale", &transforms_[Option].scale.x, 0.01f);
	}

	ImGui::Separator();

	// Quit Transform
	if (ImGui::CollapsingHeader("Quit", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Quit Position", &transforms_[Quit].position.x, 0.01f);
		ImGui::DragFloat3("Quit Rotate", &transforms_[Quit].rotate.x, 0.01f);
		ImGui::DragFloat3("Quit Scale", &transforms_[Quit].scale.x, 0.01f);
	}

	ImGui::End();
#endif
}
