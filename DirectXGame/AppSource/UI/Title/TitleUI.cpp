#include "TitleUI.h"
#include <Utility/MatrixFactory.h>
#include <Utility/Color.h>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void TitleUI::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager) {
	drawDataManager_ = drawDataManager;
	modelManager_ = modelManager;

	// モデルパスの配列
	const std::array<const char*, kUICount> modelPaths = {
		"Assets/Model/UI/Title/Logo",
		"Assets/Model/UI/Title/Start",
		"Assets/Model/UI/Title/Option",
		"Assets/Model/UI/Title/Quit"
	};

	// デバッグ名の配列
	const std::array<const char*, kUICount> debugNames = {
		"TitleUI_Logo",
		"TitleUI_Start",
		"TitleUI_Option",
		"TitleUI_Quit"
	};

	// 各UIの初期化
	for (size_t i = 0; i < kUICount; ++i) {
		// モデルの読み込み
		modelIDs_[i] = modelManager_->LoadModel(modelPaths[i]);

		// 描画設定
		renders_[i] = std::make_unique<SHEngine::RenderObject>(debugNames[i]);
		renders_[i]->Initialize();
		auto model = modelManager_->GetNodeModelData(modelIDs_[i]);
		auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
		renders_[i]->SetDrawData(drawData);
		renders_[i]->psoConfig_.vs = "Simples.VS.hlsl";
		renders_[i]->psoConfig_.ps = "TexColors.PS.hlsl";
		renders_[i]->psoConfig_.isSwapChain = false;  // displayに描画するのでfalseに変更
		renders_[i]->CreateSRV(sizeof(Matrix4x4), 1, ShaderType::VERTEX_SHADER, "WVP");
		renders_[i]->CreateSRV(sizeof(Vector4), 1, ShaderType::PIXEL_SHADER, "Color");
		renders_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		renders_[i]->SetUseTexture(true);
		renders_[i]->instanceNum_ = 1;
	}
	
	currentSelect_ = Title::Select::Start;
}

void TitleUI::UpdateSelection(bool upPressed, bool downPressed) {
	if (upPressed) {
		int currentIndex = static_cast<int>(currentSelect_);
		currentIndex--;

		if (currentIndex < 0) {
			currentIndex = static_cast<int>(Title::Select::Count) - 1;
		}
		currentSelect_ = static_cast<Title::Select>(currentIndex);
	}
	
	if (downPressed) {
		int currentIndex = static_cast<int>(currentSelect_);
		currentIndex++;

		if (currentIndex >= static_cast<int>(Title::Select::Count)) {
			currentIndex = 0;
		}
		currentSelect_ = static_cast<Title::Select>(currentIndex);
	}
}

void TitleUI::Update(const Matrix4x4& vpMatrix) {
	int textureIndex = 0;

	for (size_t i = 0; i < kUICount; ++i) {
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		
		// 選択中の項目を赤色にする（Logoは除外）
		if (i > 0) {
			Title::Kinds kind = static_cast<Title::Kinds>(i);
			Title::Select selectFromKind = static_cast<Title::Select>(static_cast<int>(kind) - 1);
			if (selectFromKind == currentSelect_) {
				color = { 1.0f, 0.0f, 0.0f, 1.0f };
			}
		}

		// WVP行列を作成
		Matrix4x4 world = Matrix::MakeAffineMatrix(scales_[i], Vector3(), positions_[i]);
		Matrix4x4 wvp = world * vpMatrix;
		
		renders_[i]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));
		renders_[i]->CopyBufferData(2, &textureIndex, sizeof(int));
	}
}

void TitleUI::Draw(CmdObj* cmdObj) {
	for (size_t i = 0; i < kUICount; ++i) {
		renders_[i]->Draw(cmdObj);
	}
}

#ifdef USE_IMGUI
void TitleUI::DrawImGui() {
	ImGui::Begin("Title UI Settings");

	const char* uiNames[] = { "Logo", "Start", "Option", "Quit" };
	const char* selectNames[] = { "Start", "Option", "Quit" };

	for (size_t i = 0; i < kUICount; ++i) {
		if (ImGui::TreeNode(uiNames[i])) {
			ImGui::DragFloat3("Position", &positions_[i].x, 0.01f);
			ImGui::DragFloat3("Scale", &scales_[i].x, 0.01f, 0.01f, 10.0f);
			ImGui::TreePop();
		}
	}
	
	ImGui::Separator();
	int currentSelectIndex = static_cast<int>(currentSelect_);
	if (ImGui::Combo("Current Selection", &currentSelectIndex, selectNames, static_cast<int>(Title::Select::Count))) {
		currentSelect_ = static_cast<Title::Select>(currentSelectIndex);
	}

	ImGui::End();
}
#endif