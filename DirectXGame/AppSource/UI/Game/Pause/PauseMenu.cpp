#include "PauseMenu.h"
#include <Utility/ConvertString.h>

using namespace SHEngine;

void PauseMenu::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, TextureManager* textureManager, Input* input, KeyManager* keyManager) {
	input_ = input;
	keyManager_ = keyManager;

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
	menuText_->SetTransform(menuTextTransform_);

	// 選択項目の背景生成&初期化
	itemBgRender_ = std::make_unique<RenderObject>("ItemBG");
	itemBgRender_->Initialize();

	// シンプルな単色シェーダーを使用
	itemBgRender_->psoConfig_.vs = "Simple.VS.hlsl";
	itemBgRender_->psoConfig_.ps = "Color.PS.hlsl";

	itemBgRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	itemBgRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	itemBgRender_->SetDrawData(data);

	itemBgTransform_.position = {0.0f, 0.0f, 0.0f};
	itemBgTransform_.rotate = {0.0f, 0.0f, 0.0f};
	itemBgTransform_.scale = {0.0f, 0.0f, 1.0f};
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

	if (input_) {
		Vector2 mousePos = keyManager_->GetCursorPos();
		mousePos.y *= -1;

		// マウスが動いたときだけホバー判定を有効にする
		bool isMouseMoved = (mousePos.x != lastMousePos_.x || mousePos.y != lastMousePos_.y);
		lastMousePos_ = mousePos;

		if (isMouseMoved) {
			int index = 0;
			for (const auto& info : infos_) {
				Vector3 pos = transforms_[info.key].position;
				float left = pos.x - hitBoxSize_.x / 2.0f;
				float right = pos.x + hitBoxSize_.x / 2.0f;
				float bottom = pos.y - hitBoxSize_.y / 2.0f;
				float top = pos.y + hitBoxSize_.y / 2.0f;

				// マウスがテキストの枠内に入っていたら選択を上書き
				if (mousePos.x >= left && mousePos.x <= right && mousePos.y >= bottom && mousePos.y <= top) {
					selectedIndex_ = index;
					break;
				}
				index++;
			}
		}

		if (key[Key::Tr_LeftClick]) {
			if (actions_[selectedIndex_]) {
				actions_[selectedIndex_]();
			}

			AudioManager::GetInstance()->GetData("BackPackMove.mp3")->Play();
		}
	}

	int index = 0;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	// テキスト更新
	for (const auto& info : infos_) {
		// 選択中ならサイズを大きくする
		if (index == selectedIndex_) {
			transforms_[info.key].scale = {slectedSize_, slectedSize_, 1.0f};
			color = selectColor_;
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
	ImGui::Begin("UI");
	ImGui::ColorEdit4("selectColor", &selectColor_.x);
	ImGui::End();
#endif

	// メニュー画面表記更新
	menuText_->Update(vpMatrix);

	// 背景のWVP行列を更新
	backgroundWVP_ = Matrix::MakeAffineMatrix(backgroundTransform_.scale, backgroundTransform_.rotate, backgroundTransform_.position);
	backgroundWVP_ *= vpMatrix;
	backgroundRender_->CopyBufferData(0, &backgroundWVP_, sizeof(Matrix4x4));

	Vector4 bgColor = {0.0f, 0.0f, 0.0f, 0.9f};
	backgroundRender_->CopyBufferData(1, &bgColor, sizeof(Vector4));

	// 選択中のインデックスが変わった瞬間にアニメーションを開始する
	if (selectedIndex_ != previousSelectedIndex_) {
		previousSelectedIndex_ = selectedIndex_;

		// イージング
		Vector3 startScale = {0.0f, 0.0f, 1.0f};
		Vector3 endScale = {itemBgSize_.x, itemBgSize_.y, 1.0f};

		scaleAnim_.anim.Start(startScale, endScale, 0.2f, EaseType::EaseOutBack);
	}

	// アニメーションの更新
	scaleAnim_.anim.Update(deltaTime, scaleAnim_.temp);
	itemBgTransform_.scale = scaleAnim_.temp;

	// 背景の座標を選択中のテキストの座標に合わせる
	std::wstring selectedKey = infos_[selectedIndex_].key;
	itemBgTransform_.position = transforms_[selectedKey].position;
	itemBgWVP_ = Matrix::MakeAffineMatrix(itemBgTransform_.scale, itemBgTransform_.rotate, itemBgTransform_.position);
	itemBgWVP_ *= vpMatrix;
	itemBgRender_->CopyBufferData(0, &itemBgWVP_, sizeof(Matrix4x4));
	itemBgRender_->CopyBufferData(1, &itemBgColor_, sizeof(Vector4));

	if (selectedIndex_ != lastSelectedIndex_) {
		AudioManager::GetInstance()->GetData("ItemSelect.mp3")->Play();
		lastSelectedIndex_ = selectedIndex_;
	}

#ifdef USE_IMGUI
	ImGui::Begin("ItemBg");
	ImGui::DragFloat2("Size", &itemBgSize_.x, 1.0f);
	ImGui::End();
#endif
}

void PauseMenu::Draw(CmdObj* cmdObj) {
	// 背景描画
	backgroundRender_->Draw(cmdObj);

	// 選択項目の背景描画
	itemBgRender_->Draw(cmdObj);

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
