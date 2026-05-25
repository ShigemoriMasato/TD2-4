#include "PauseMenu.h"
#include <Utility/ConvertString.h>

using namespace SHEngine;

void PauseMenu::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, TextureManager* textureManager) {
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);
	int index = 0;

	// 各種テキストの生成、初期化
	for (const auto& info : infos_) {
		CreateText(info.key, data, "YDWbananaslipplus.otf", 64, info.name, info.text);
		transforms_[info.key].position.x = 100.0f;
		transforms_[info.key].position.y = index * -100.0f - 200.0f;
		index++;
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
	backgroundRender_->SetDrawData(data);

	// Transform
	backgroundTransform_.position = {0.0f, 0.0f, 0.0f};
	backgroundTransform_.rotate = {0.0f, 0.0f, 0.0f};
	backgroundTransform_.scale = {2560.0f, 1440.0f, 0.0f};

	// メニュー画面表記
	menuText_ = std::make_unique<Text>();
	menuText_->Initialize(data, "YDWbananaslipplus.otf", 128, "MenuText");
	menuText_->SetText(L"メニュー画面");
	menuTextTransform_.position = {500.0f, -100.0f, 0.0f};
}

void PauseMenu::Update(Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key) {
	// キー入力
	if (key[Key::Tr_Up]) {
		selectedIndex_--;
		if (selectedIndex_ < 0)
			selectedIndex_ = (int)infos_.size() - 1;
	}
	if (key[Key::Tr_Down]) {
		selectedIndex_++;
		if (selectedIndex_ >= infos_.size())
			selectedIndex_ = 0;
	}
	if (key[Key::Correct]) {
		if (actions_.count(selectedIndex_)) {
			actions_[selectedIndex_]();
		}
	}

	int index = 0;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	// テキスト更新
	for (const auto& info : infos_) {
		// 選択中ならサイズを大きくする
		if (index == selectedIndex_) {
			transforms_[info.key].scale = {slectedSize_, slectedSize_, 1.0f};
			color = {1.0f, 0.0f, 0.0f, 1.0f};
		} else {
			transforms_[info.key].scale = {normalSize_, normalSize_, 1.0f};
			color = {1.0f, 1.0f, 1.0f, 1.0f};
		}

		texts_[info.key]->SetColor(color);
		texts_[info.key]->SetTransform(transforms_[info.key]);
		texts_[info.key]->Update(vpMatrix);

		index++;

#ifdef USE_IMGUI
		ImGui::Begin("PauseMenu");

		std::string utf8Key = ConvertString(info.key);

		ImGui::PushID(utf8Key.c_str());

		ImGui::Text("Key: %s", utf8Key.c_str());
		ImGui::DragFloat3("Position", &transforms_[info.key].position.x, 1.0f);

		ImGui::PopID();

		ImGui::End();
#endif
	}

#ifdef USE_IMGUI
	ImGui::Begin("MenuText");
	ImGui::DragFloat3("Pos", &menuTextTransform_.position.x, 1.0f);
	menuText_->SetTransform(menuTextTransform_);
	ImGui::End();
#endif

	// メニュー画面表記更新
	menuText_->Update(vpMatrix);

	// 背景のWVP行列を更新
	backgroundWVP_ = Matrix::MakeAffineMatrix(backgroundTransform_.scale, backgroundTransform_.rotate, backgroundTransform_.position);
	backgroundWVP_ *= vpMatrix;
	backgroundRender_->CopyBufferData(0, &backgroundWVP_, sizeof(Matrix4x4));

	Vector4 bgColor = {0.0f, 0.0f, 0.0f, 0.8f};
	backgroundRender_->CopyBufferData(1, &bgColor, sizeof(Vector4));
}

void PauseMenu::Draw(CmdObj* cmdObj) {
	// 背景描画
	backgroundRender_->Draw(cmdObj);

	// メニュー画面表記描画
	menuText_->Draw(cmdObj);

	// テキストの描画
	for (const auto& info : infos_) {
		texts_[info.key]->Draw(cmdObj);
	}
}

void PauseMenu::CreateText(const std::wstring& key, DrawData& drawData, const std::string& font, int fontSize, const std::string& name, const std::wstring& text) {
	auto t = std::make_unique<Text>();
	t->Initialize(drawData, font, fontSize, name);
	t->SetText(text);
	texts_[key] = std::move(t);
}
