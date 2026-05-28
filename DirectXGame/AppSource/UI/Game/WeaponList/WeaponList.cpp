#include "WeaponList.h"
#include <Utility/ConvertString.h>
#include <utility>

using namespace SHEngine;

void WeaponList::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, KeyManager* keyManager, Input* input) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);

	keyManager_ = keyManager;
	input_ = input;

	// 背景のTransform配列
	bgTransforms_.resize(kBGSpriteCount);

	// 武器のTransform配列
	weaponTransforms_.resize(kWeaponCount);

	// 背景色
	bgColors_.resize(kBGSpriteCount);

	// 武器テキスト開始位置
	weaponTextStartPos_ = {240.0f, -240.0f, 0.0f};

	// 武器テキストの開始位置
	weaponTextStartPos_ = {240.0f, -240.0f, 0.0f};

	// 最大スクロール量の計算
	maxScrollOffset_ = std::max(0.0f, (kWeaponCount - kVisibleItemCount) * kItemHeight);

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
	// スクロール
	float scrollDelta = -input_->GetMouseWheel();

	// スクロール感度
	float scrollSpeed = 24.0f;

	if (scrollDelta > 0.0f) {
		scrollOffset_ -= scrollSpeed;
	} else if (scrollDelta < 0.0f) {
		scrollOffset_ += scrollSpeed;
	}

	// スクロール量のクランプ
	scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScrollOffset_);

	// 武器テキストの更新
	for (int i = 0; i < kWeaponCount; ++i) {
		weaponTransforms_[i].position.y = weaponTextStartPos_.y - (i * kItemHeight);
		weaponTexts_[i]->SetTransform(weaponTransforms_[i]);
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

	// 枠外の判定を無効化
	float topLimit = weaponTextStartPos_.y + (kItemHeight / 2.0f);
	float bottomLimit = weaponTextStartPos_.y - ((kVisibleItemCount - 1) * kItemHeight) - (kItemHeight / 2.0f);

	Vector2 mousePos = keyManager_->GetCursorPos();
	mousePos.y *= -1;

	// 左クリックが押されたか判定
	if (key[Key::Tr_LeftClick]) {
		for (int i = 0; i < kWeaponCount; ++i) {
			float posY = weaponTransforms_[i].position.y;

			//  テキストが枠外にある場合はクリック判定をスキップ
			if (posY > topLimit || posY < bottomLimit) {
				continue;
			}

			// テキストの座標を中心に、当たり判定用の矩形を計算
			Vector3 pos = weaponTransforms_[i].position;
			float left = pos.x;
			float right = pos.x + hitBoxSize_.x;
			float top = pos.y;
			float bottom = pos.y - hitBoxSize_.y;

			// マウス座標が矩形の中に入っているか判定
			if (mousePos.x >= left && mousePos.x <= right && mousePos.y >= bottom && mousePos.y <= top) {

				// 選択中の武器IDを更新
				selectedWeaponId_ = i;

				// 選択中武器名テキスト更新
				selectWeaponText_->SetText(weaponNames_[selectedWeaponId_]);

				break;
			}
		}
	}

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

	// 表示範囲の上下限を計算
	float topLimit = weaponTextStartPos_.y + (kItemHeight / 2.0f);
	float bottomLimit = weaponTextStartPos_.y - ((kVisibleItemCount - 1) * kItemHeight) - (kItemHeight / 2.0f);

	// 武器テキストの描画
	for (int i = 0; i < kWeaponCount; ++i) {
		float posY = weaponTransforms_[i].position.y;

		// 座標が枠内に収まっている項目のみ描画する
		if (posY <= topLimit && posY >= bottomLimit) {
			weaponTexts_[i]->Draw(cmdObj);
		}
	}
}

std::unique_ptr<Text> WeaponList::AddText(const std::wstring& textName, DrawData& data, const std::string& fontPath, int fontSize, const std::string& debugName) {
	std::unique_ptr<Text> text;
	text = std::make_unique<Text>();
	text->Initialize(data, fontPath, fontSize, debugName);
	text->SetText(textName);

	return text;
}
