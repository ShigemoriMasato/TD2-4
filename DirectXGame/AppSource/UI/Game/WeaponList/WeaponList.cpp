#include "WeaponList.h"
#include "GameObject/Weapon/WeaponDatabase.h"
#include <Utility/ConvertString.h>
#include <format>
#include <numbers>
#include <utility>

using namespace SHEngine;

void WeaponList::Initialize(
    SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, KeyManager* keyManager, Input* input, WeaponDatabase* weaponDatabase) {
	textureManager_ = textureManager;

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

	// テクスチャインデックス
	textureIndexArray_.resize(kBGSpriteCount);

	for (int i = 0; i < kBGSpriteCount; ++i) {
		if (i == static_cast<int>(BGType::CloseButton)) {
			textureIndexArray_[i] = textureManager_->LoadTexture("CloseIcon.png");
		} else {
			textureIndexArray_[i] = 0;
		}
	}

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
	bgRenders_->psoConfig_.ps = "Game/WeaponListBG.PS.hlsl";
	bgRenders_->CreateSRV(sizeof(Matrix4x4), kBGSpriteCount, ShaderType::VERTEX_SHADER, "WVP");
	bgRenders_->CreateSRV(sizeof(Vector4), kBGSpriteCount, ShaderType::PIXEL_SHADER, "Color");
	bgRenders_->CreateSRV(sizeof(int), kBGSpriteCount, ShaderType::PIXEL_SHADER, "TextureIndex");
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

	// 閉じるボタン
	index = static_cast<int>(BGType::CloseButton);
	bgTransforms_[index].scale = {128.0f, 128.0f, 0.0f};
	bgTransforms_[index].rotate = {0.0f, 0.0f, 0.0f};
	bgTransforms_[index].position = {96.0f, -96.0f, 0.0f};
	bgColors_[index] = {1.0f, 1.0f, 1.0f, 1.0f};

	// スクロールバートラック
	index = static_cast<int>(BGType::ScrollbarTrack);
	bgTransforms_[index].scale = {16.0f, 400.0f, 0.0f}; // リストの高さに合わせる
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {580.0f, -400.0f, 0}; // 武器一覧UIの右端に配置
	bgColors_[index] = {0.2f, 0.2f, 0.2f, 1.0f};

	// スクロールバーノブ
	index = static_cast<int>(BGType::ScrollbarKnob);
	bgTransforms_[index].scale = {16.0f, 60.0f, 0.0f};
	bgTransforms_[index].rotate = {0, 0, 0};
	bgTransforms_[index].position = {580.0f, 0.0f, 0};
	bgColors_[index] = {0.8f, 0.8f, 0.8f, 1.0f};

	// 武器一覧テキスト
	allWeaponText_ = AddText(L"武器一覧", data, "YDWbananaslipplus.otf", 64, "AllWeapons");
	allWeaponsTransform_.position.x = 230.0f;
	allWeaponsTransform_.position.y = -165.0f;

	// 選択中武器テキスト
	selectWeaponText_ = AddText(weaponNames_[0], data, "YDWbananaslipplus.otf", 64, "SelectWeapon");
	selectWeaponTransform_.position.x = 780.0f;
	selectWeaponTransform_.position.y = -165.0f;

	// 武器モデルの読み込みと初期化
	std::array<std::string, kWeaponCount> modelNames = {"Sword", "Pistol", "Spear", "Shotgun", "Axe", "Bow", "Fist", "Gurepon", "Pickaxe", "Shuriken"};

	weaponModels_.resize(kWeaponCount);
	weaponTextureIndices_.resize(kWeaponCount);

	for (int i = 0; i < kWeaponCount; ++i) {
		weaponModels_[i] = std::make_unique<SHEngine::RenderObject>("WeaponModel_" + modelNames[i]);
		weaponModels_[i]->Initialize();
		weaponModels_[i]->psoConfig_.vs = "Game/Field.VS.hlsl";
		weaponModels_[i]->psoConfig_.ps = "Game/Field.PS.hlsl";
		weaponModels_[i]->SetUseTexture(true);

		std::string modelPath = "Assets/Model/Item/Weapon/" + modelNames[i];
		int modelHandle = modelManager->LoadModel(modelPath);
		auto modelData = modelManager->GetNodeModelData(modelHandle);
		auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		weaponModels_[i]->SetDrawData(drawData);

		auto& material = modelData.materials[modelData.materialIndex.front()];
		weaponTextureIndices_[i] = material.textureIndex;

		weaponModels_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		weaponModels_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER);
		weaponModels_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER);
		weaponModels_[i]->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER);
	}

	// 表示位置の初期設定
	weaponModelTransform_.position = {0.49f, 0.0f, -1.0f};
	weaponModelTransform_.rotate = {-std::numbers::pi_v<float> / 2.0f, 0.0f, 0.0f};
	weaponModelTransform_.scale = {0.07f, 0.07f, 0.07f};

	// 武器背景用の配列をリサイズ
	weaponBGTransforms_.resize(kWeaponCount);
	weaponBGColors_.resize(kWeaponCount);

	weaponBGRenders_ = std::make_unique<RenderObject>("WeaponItemBG");
	weaponBGRenders_->Initialize();
	weaponBGRenders_->SetDrawData(data);
	weaponBGRenders_->psoConfig_.vs = "Simples.VS.hlsl";
	weaponBGRenders_->psoConfig_.ps = "TexColors.PS.hlsl";
	weaponBGRenders_->CreateSRV(sizeof(Matrix4x4), kWeaponCount, ShaderType::VERTEX_SHADER, "WVP");
	weaponBGRenders_->CreateSRV(sizeof(Vector4), kWeaponCount, ShaderType::PIXEL_SHADER, "Color");
	weaponBGRenders_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	weaponBGRenders_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");
	weaponBGRenders_->SetUseTexture(true);
	weaponBGRenders_->instanceNum_ = kWeaponCount;

	// 初期サイズとデフォルトの色を設定
	for (int i = 0; i < kWeaponCount; ++i) {
		weaponBGTransforms_[i].scale = {hitBoxSize_.x, hitBoxSize_.y, 1.0f};
		weaponBGTransforms_[i].rotate = {0.0f, 0.0f, 0.0f};
		weaponBGTransforms_[i].position = {0.0f, 0.0f, 0.0f};
		weaponBGColors_[i] = kNormalBGColor;
	}

	// 選択中武器のステータステキストの生成
	selectDamageTexts_.resize(kWeaponCount);
	selectSpeedTexts_.resize(kWeaponCount);

	for (int i = 0; i < kWeaponCount; ++i) {
		const WeaponData* weaponData = weaponDatabase->GetWeapon(i);
		std::wstring dmgStr = L"ATK: 0.0";
		std::wstring spdStr = L"SPD: 0.0";

		if (weaponData) {
			dmgStr = std::format(L"ATK: {:.1f}", weaponData->baseDamage);
			spdStr = std::format(L"SPD: {:.2f}", weaponData->attackSpeed);
		}

		selectDamageTexts_[i] = AddText(dmgStr, data, "YDWbananaslipplus.otf", 64, "SelectDamage_" + std::to_string(i));
		selectSpeedTexts_[i] = AddText(spdStr, data, "YDWbananaslipplus.otf", 64, "SelectSpeed_" + std::to_string(i));
	}

	allWeaponText_->SetTransform(allWeaponsTransform_);
	selectWeaponText_->SetTransform(selectWeaponTransform_);
}

void WeaponList::Update(Matrix4x4 orthoVpMatrix, Matrix4x4 vpMatrix, float deltaTime, std::unordered_map<Key, bool> key) {
	// スクロール
	float scrollDelta = input_->GetMouseWheel();

	// スクロール感度
	float scrollSpeed = 72.0f;

	if (scrollDelta > 0.0f) {
		scrollOffset_ -= scrollSpeed;
	} else if (scrollDelta < 0.0f) {
		scrollOffset_ += scrollSpeed;
	}

	// スクロール量のクランプ
	scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScrollOffset_);

	// 武器テキストの更新
	for (int i = 0; i < kWeaponCount; ++i) {
		weaponTransforms_[i].position.y = weaponTextStartPos_.y - (i * kItemHeight) + scrollOffset_;
		weaponTexts_[i]->SetTransform(weaponTransforms_[i]);
		weaponTexts_[i]->Update(orthoVpMatrix);
	}

	Vector2 mousePos = keyManager_->GetCursorPos();
	mousePos.y *= -1;

	// 枠外の判定を無効化
	float topLimit = weaponTextStartPos_.y + (kItemHeight / 2.0f);
	float bottomLimit = weaponTextStartPos_.y - ((kVisibleItemCount - 1) * kItemHeight) - (kItemHeight / 2.0f);

	// 閉じるボタンのクリック処理
	int closeBtnIndex = static_cast<int>(BGType::CloseButton);
	Vector3 cbPos = bgTransforms_[closeBtnIndex].position;
	Vector3 cbScale = bgTransforms_[closeBtnIndex].scale;

	// 中心が原点であると仮定して、当たり判定用の矩形を計算
	float cbLeft = cbPos.x - (cbScale.x / 2.0f);
	float cbRight = cbPos.x + (cbScale.x / 2.0f);
	float cbTop = cbPos.y + (cbScale.y / 2.0f);
	float cbBottom = cbPos.y - (cbScale.y / 2.0f);

	// カーソルがボタンと被っているか判定
	bool isHover = (mousePos.x >= cbLeft && mousePos.x <= cbRight && mousePos.y >= cbBottom && mousePos.y <= cbTop);

	if (isHover) {
		// カーソルが被っているときは色を変える
		bgColors_[closeBtnIndex] = {0.6f, 0.6f, 0.6f, 1.0f};

		// 左クリックされたときの処理
		if (key[Key::Tr_LeftClick]) {
			// クリック時は元の色に戻す
			bgColors_[closeBtnIndex] = {1.0f, 1.0f, 1.0f, 1.0f};

			// 登録されたコールバックを呼ぶ
			if (closeAction_) {
				closeAction_();
			}
		}
	} else {
		// 被っていないときは元の色に戻す
		bgColors_[closeBtnIndex] = {1.0f, 1.0f, 1.0f, 1.0f};
	}

	// 左クリックが押されたか判定
	if (key[Key::Tr_LeftClick]) {
		for (int i = 0; i < kWeaponCount; ++i) {
			float posY = weaponTransforms_[i].position.y;

			//  テキストが枠外にある場合はクリック判定をスキップ
			if (posY > topLimit || posY < bottomLimit) {
				continue;
			}

			// テキストの座標を中心に、当たり判定用の矩形を計算
			Vector3 pos = {weaponTransforms_[i].position.x + textBGMargin_.x, weaponTransforms_[i].position.y + textBGMargin_.y, weaponTransforms_[i].position.z};
			float left = pos.x - (hitBoxSize_.x / 2.0f);
			float right = pos.x + (hitBoxSize_.x / 2.0f);
			float top = pos.y + (hitBoxSize_.y / 2.0f);
			float bottom = pos.y - (hitBoxSize_.y / 2.0f);

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
		wvp *= orthoVpMatrix;
		wvpMatrices.push_back(wvp);
	}

	// バッファコピー
	bgRenders_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * wvpMatrices.size());
	bgRenders_->CopyBufferData(1, bgColors_.data(), sizeof(Vector4) * bgColors_.size());
	bgRenders_->CopyBufferData(2, textureIndexArray_.data(), sizeof(int) * textureIndexArray_.size());
	bgRenders_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));

	// スクロールバーのパラメータ
	float trackHeight = 400.0f;
	float trackCenterY = -400.0f;
	float knobHeight = 60.0f;
	float movableRange = trackHeight - knobHeight;
	float trackTopY = trackCenterY + (movableRange / 2.0f);

	int knobIndex = static_cast<int>(BGType::ScrollbarKnob);
	Vector3 knobPos = bgTransforms_[knobIndex].position;
	Vector3 knobScale = bgTransforms_[knobIndex].scale;

	// ノブの当たり判定用矩形
	float knobLeft = knobPos.x;
	float knobRight = knobPos.x + knobScale.x;
	float knobTop = knobPos.y + (knobScale.y / 2.0f);
	float knobBottom = knobPos.y - (knobScale.y / 2.0f);

	// マウスがノブにホバーしているか
	bool isHoverKnob = (mousePos.x >= knobLeft && mousePos.x <= knobRight && mousePos.y >= knobBottom && mousePos.y <= knobTop);

	// 左クリック長押し
	bool isLeftClickHeld = key[Key::Hold];

	// ドラッグ開始判定
	if (isHoverKnob) {
		bgColors_[knobIndex] = {1.0f, 1.0f, 1.0f, 1.0f};
		if (key[Key::Tr_LeftClick]) {
			isDraggingScrollbar_ = true;
			dragStartY_ = mousePos.y;
			dragStartScrollOffset_ = scrollOffset_;
		}
	} else if (!isDraggingScrollbar_) {
		bgColors_[knobIndex] = {0.8f, 0.8f, 0.8f, 1.0f};
	}

	// ドラッグ中の処理
	if (isDraggingScrollbar_) {
		bgColors_[knobIndex] = {0.6f, 0.6f, 0.6f, 1.0f};

		if (isLeftClickHeld) {
			// マウスの移動量からスクロールの増加量を計算
			float dragDeltaY = mousePos.y - dragStartY_;

			// ノブの移動量をcrollOffset_の変化量に変換
			float rawScrollAmount = -dragDeltaY * (maxScrollOffset_ / movableRange);

			// 72.0f単位にスナップ
			float snappedScroll = std::round(rawScrollAmount / 72.0f) * 72.0f;

			scrollOffset_ = dragStartScrollOffset_ + snappedScroll;
		} else {
			// 左クリックが離されたらドラッグ終了
			isDraggingScrollbar_ = false;
		}
	}

	// スクロール量のクランプ
	scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScrollOffset_);

	// ノブのY座標を更新
	float scrollRatio = (maxScrollOffset_ > 0.0f) ? (scrollOffset_ / maxScrollOffset_) : 0.0f;
	bgTransforms_[knobIndex].position.y = trackTopY - (scrollRatio * movableRange);

	for (int i = 0; i < kWeaponCount; ++i) {
		// 各アイテムの現在のY座標を計算
		float itemY = weaponTextStartPos_.y - (i * kItemHeight) + scrollOffset_;

		// 矩形の座標を設定
		weaponBGTransforms_[i].position = {weaponTextStartPos_.x + textBGMargin_.x, itemY + textBGMargin_.y, 0.0f};

		// 画面外にある場合は、判定を行わず透明にする
		if (itemY > topLimit || itemY < bottomLimit) {
			weaponBGColors_[i] = {0.0f, 0.0f, 0.0f, 0.0f}; // アルファを0にして消す
			continue;
		}

		// 矩形の当たり判定用の4辺を計算
		float left = weaponBGTransforms_[i].position.x - (hitBoxSize_.x / 2.0f);
		float right = weaponBGTransforms_[i].position.x + (hitBoxSize_.x / 2.0f);
		float top = weaponBGTransforms_[i].position.y + (hitBoxSize_.y / 2.0f);
		float bottom = weaponBGTransforms_[i].position.y - (hitBoxSize_.y / 2.0f);

		// マウスカーソルが矩形の内側にあるか判定
		if (mousePos.x >= left && mousePos.x <= right && mousePos.y >= bottom && mousePos.y <= top) {
			// カーソルが重なっているならホバー色
			weaponBGColors_[i] = kHoverBGColor;
		} else {
			// 重なっていないなら通常色
			weaponBGColors_[i] = kNormalBGColor;
		}
	}

	std::vector<Matrix4x4> wvpMat;
	wvpMat.reserve(kWeaponCount);

	for (int i = 0; i < kWeaponCount; ++i) {
		Matrix4x4 wvp = Matrix::MakeAffineMatrix(weaponBGTransforms_[i].scale, weaponBGTransforms_[i].rotate, weaponBGTransforms_[i].position);
		wvp *= orthoVpMatrix;
		wvpMat.push_back(wvp);
	}

	int textureIndex = 0;

	weaponBGRenders_->CopyBufferData(0, wvpMat.data(), sizeof(Matrix4x4) * wvpMat.size());
	weaponBGRenders_->CopyBufferData(1, weaponBGColors_.data(), sizeof(Vector4) * weaponBGColors_.size());
	weaponBGRenders_->CopyBufferData(2, &textureIndex, sizeof(int));
	weaponBGRenders_->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));

	// 武器一覧テキスト更新
	allWeaponText_->Update(orthoVpMatrix);

	// 選択中武器テキスト
	selectWeaponText_->Update(orthoVpMatrix);

	// モデルをゆっくり回転させる
	modelRotationY_ += deltaTime * 1.5f;
	weaponModelTransform_.rotate.y = modelRotationY_;

	// 行列の計算
	Matrix4x4 worldMatrix = Matrix::MakeAffineMatrix(weaponModelTransform_.scale, weaponModelTransform_.rotate, weaponModelTransform_.position);
	Matrix4x4 wvp = worldMatrix * vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	// 選択中の武器インデックス
	int currentIndex = selectedWeaponId_;
	int texIndex = weaponTextureIndices_[currentIndex];

	weaponModels_[currentIndex]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	weaponModels_[currentIndex]->CopyBufferData(1, &color, sizeof(Vector4));
	weaponModels_[currentIndex]->CopyBufferData(2, &texIndex, sizeof(int));
	weaponModels_[currentIndex]->CopyBufferData(3, &dirLight_, sizeof(DirectionalLight));

	// 選択中の武器名テキストのTransformをベースに位置を計算
	selectDamageTransform_ = selectWeaponTransform_;
	selectDamageTransform_.position.y += dmgOffsetY_; // オフセットを加算

	selectSpeedTransform_ = selectWeaponTransform_;
	selectSpeedTransform_.position.y += spdOffsetY_; // オフセットを加算

	// 現在選択されている武器のステータステキストのみ行列を更新
	if (selectedWeaponId_ >= 0 && selectedWeaponId_ < kWeaponCount) {
		selectDamageTexts_[selectedWeaponId_]->SetTransform(selectDamageTransform_);
		selectDamageTexts_[selectedWeaponId_]->Update(orthoVpMatrix);

		selectSpeedTexts_[selectedWeaponId_]->SetTransform(selectSpeedTransform_);
		selectSpeedTexts_[selectedWeaponId_]->Update(orthoVpMatrix);
	}
}

void WeaponList::Draw(CmdObj* cmdObj) {
	// 背景の描画
	bgRenders_->Draw(cmdObj);

	// 武器項目の背景矩形を描画
	weaponBGRenders_->Draw(cmdObj);

	// 武器一覧テキスト描画
	allWeaponText_->Draw(cmdObj);

	// 選択中武器テキスト
	selectWeaponText_->Draw(cmdObj);

	// 選択されている武器のステータステキストを描画
	if (selectedWeaponId_ >= 0 && selectedWeaponId_ < kWeaponCount) {
		selectDamageTexts_[selectedWeaponId_]->Draw(cmdObj);
		selectSpeedTexts_[selectedWeaponId_]->Draw(cmdObj);
	}

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

	// モデル描画
	weaponModels_[selectedWeaponId_]->Draw(cmdObj);
}

std::unique_ptr<Text> WeaponList::AddText(const std::wstring& textName, DrawData& data, const std::string& fontPath, int fontSize, const std::string& debugName) {
	std::unique_ptr<Text> text;
	text = std::make_unique<Text>();
	text->Initialize(data, fontPath, fontSize, debugName);
	text->SetText(textName);

	return text;
}
