#include "WeaponList.h"
#include <Utility/ConvertString.h>
#include <utility>

using namespace SHEngine;

void WeaponList::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);

	// 背景のTransform配列
	bgTransforms_.resize(kBGSpriteCount);

	// 武器のTransform配列
	weaponTransforms_.resize(kWeaponCount);

	// 背景色
	bgColors_.resize(kBGSpriteCount);

	// 武器テキスト開始位置
	weaponTextStartPos_ = {240.0f, -240.0f, 0.0f};

	// 武器テキストの追加
	for (int i = 0; i < kWeaponCount; ++i) {
		weaponTexts_.push_back(std::move(AddText(weaponNames_[i], data, "YDWbananaslipplus.otf", 64, ConvertString(weaponNames_[i]))));

		if (i == 0) {
			weaponTransforms_[i].position = weaponTextStartPos_;
		} else {
			weaponTransforms_[i].position = weaponTransforms_[i - 1].position;
			weaponTransforms_[i].position.y -= textMarginY_;
		}
	}

	// 背景の初期化
	bgRenders_ = std::make_unique<RenderObject>("GaugeUI");
	bgRenders_->Initialize();
	bgRenders_->SetDrawData(data);
	bgRenders_->psoConfig_.vs = "Simples.VS.hlsl";
	bgRenders_->psoConfig_.ps = "TexColors.PS.hlsl";
	bgRenders_->CreateSRV(sizeof(Matrix4x4), kBGSpriteCount, ShaderType::VERTEX_SHADER, "WVP");
	bgRenders_->CreateSRV(sizeof(Vector4), kBGSpriteCount, ShaderType::PIXEL_SHADER, "Color");
	bgRenders_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	bgRenders_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");
	bgRenders_->SetUseTexture(true);
	bgRenders_->instanceNum_ = kBGSpriteCount;

	// 全体
	int index = static_cast<int>(BGType::FullBG);
	bgTransforms_[index].scale = {2560.0f, 1440.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {0, 0, 0};
	bgColors_[index] = {0.8f, 0.8f, 0.8f, 1};

	// 武器一覧
	index = static_cast<int>(BGType::AllWeaponsBG);
	bgTransforms_[index].scale = {400.0f, 500.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {400.0f, -370.0f, 0};
	bgColors_[index] = {0.6f, 0.6f, 0.6f, 1};

	// 武器一覧
	index = static_cast<int>(BGType::SelectWeaponBG);
	bgTransforms_[index].scale = {400.0f, 500.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {950.0f, -370.0f, 0};
	bgColors_[index] = {0.6f, 0.6f, 0.6f, 1};

	// 武器項目
	index = static_cast<int>(BGType::WeaponName);
	bgTransforms_[index].scale = {400.0f, 70.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {400.0f, -155.0f, 0};
	bgColors_[index] = {0, 0, 0, 1};

	// 選択中武器名
	index = static_cast<int>(BGType::SelectWeaponName);
	bgTransforms_[index].scale = {400.0f, 70.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {950.0f, -155.0f, 0};
	bgColors_[index] = {0, 0, 0, 1};

	// 武器一覧テキスト
	allWeaponText_ = AddText(L"武器一覧", data, "YDWbananaslipplus.otf", 64, "AllWeapons");
	allWeaponsTransform_.position.x = 230.0f;
	allWeaponsTransform_.position.y = -165.0f;

	// 選択中武器テキスト
	selectWeaponText_ = AddText(weaponNames_[0], data, "YDWbananaslipplus.otf", 64, "SelectWeapon");
	selectWeaponTransform_.position.x = 780.0f;
	selectWeaponTransform_.position.y = -165.0f;
}

void WeaponList::Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key) {
	// 背景の更新
	std::vector<Matrix4x4> wvpMatrices;
	wvpMatrices.reserve(kBGSpriteCount);

	for (int i = 0; i < kBGSpriteCount; ++i) {
		Matrix4x4 wvp = Matrix::MakeAffineMatrix(bgTransforms_[i].scale, bgTransforms_[i].rotate, bgTransforms_[i].position);
		Vector4 color = {1, 1, 1, 1};

		wvp *= vpMatrix;

		wvpMatrices.push_back(wvp);
	}

	// バッファコピー
	bgRenders_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * wvpMatrices.size());
	bgRenders_->CopyBufferData(1, bgColors_.data(), sizeof(Vector4) * bgColors_.size());
	bgRenders_->CopyBufferData(2, &textureIndex_, sizeof(int));
	bgRenders_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));

	// 武器テキストの更新
	for (int i = 0; i < kWeaponCount; ++i) {
		weaponTexts_[i]->Update(vpMatrix);

#ifdef USE_IMGUI
		ImGui::Begin("WeaponTexts");
		std::string key = ConvertString(weaponNames_[i]);

		ImGui::PushID(key.c_str());

		ImGui::Text("Key: %s", key.c_str());
		ImGui::DragFloat3("Scale", &weaponTransforms_[i].scale.x, 1.0f);
		ImGui::DragFloat3("Position", &weaponTransforms_[i].position.x, 1.0f);

		ImGui::PopID();
		ImGui::End();

		weaponTexts_[i]->SetTransform(weaponTransforms_[i]);
#endif
	}

	for (int i = 0; i < kBGSpriteCount; ++i) {
#ifdef USE_IMGUI
		// 背景スプライトImGui
		ImGui::Begin("BGRenders");
		std::string key = bgSpriteNames_[i];

		ImGui::PushID(key.c_str());

		ImGui::Text("Key: %s", key.c_str());
		ImGui::DragFloat3("Scale", &bgTransforms_[i].scale.x, 1.0f);
		ImGui::DragFloat3("Position", &bgTransforms_[i].position.x, 1.0f);

		ImGui::PopID();
		ImGui::End();
#endif
	}

	// 武器一覧テキスト更新
	allWeaponText_->Update(vpMatrix);

	// 選択中武器テキスト
	selectWeaponText_->Update(vpMatrix);

#ifdef USE_IMGUI
	ImGui::Begin("Texts");
	ImGui::PushID("AllWeapon");
	ImGui::DragFloat3("Scale", &allWeaponsTransform_.scale.x, 1.0f);
	ImGui::DragFloat3("Pos", &allWeaponsTransform_.position.x, 1.0f);
	ImGui::PopID();

	ImGui::PushID("SelectWeapon");
	ImGui::DragFloat3("Scale", &selectWeaponTransform_.scale.x, 1.0f);
	ImGui::DragFloat3("Pos", &selectWeaponTransform_.position.x, 1.0f);
	ImGui::PopID();
	ImGui::End();

	allWeaponText_->SetTransform(allWeaponsTransform_);
	selectWeaponText_->SetTransform(selectWeaponTransform_);
#endif
}

void WeaponList::Draw(CmdObj* cmdObj) {
	// 背景の描画
	bgRenders_->Draw(cmdObj);

	// 武器一覧テキスト描画
	allWeaponText_->Draw(cmdObj);

	// 選択中武器テキスト
	selectWeaponText_->Draw(cmdObj);

	// 武器テキストの描画
	for (auto& text : weaponTexts_) {
		text->Draw(cmdObj);
	}
}

std::unique_ptr<Text> WeaponList::AddText(const std::wstring& textName, DrawData& data, const std::string& fontPath, int fontSize, const std::string& debugName) {
	std::unique_ptr<Text> text;
	text = std::make_unique<Text>();
	text->Initialize(data, fontPath, fontSize, debugName);
	text->SetText(textName);

	return text;
}
