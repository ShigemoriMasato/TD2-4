#include"OreItemManager.h"

// 各鉱石
#include"Object/GoldOre.h"

void OreItemManager::Initialize(const DrawData& goldOreDrawData, int texture) {

	// 金鉱石の描画データを取得
	goldOreDrawData_ = goldOreDrawData;
	oreTexture_ = texture;

	// メモリを確保
	oreItems_.reserve(20);

	// アウトライン描画用
	oreOutLineObject_ = std::make_unique<OreOutLineObject>();
	oreOutLineObject_->Initialize(goldOreDrawData);
}

void OreItemManager::Update() {

	// 鉱石を削除
	for (auto it = graveyard_.begin(); it != graveyard_.end(); ) {
		// カウントダウン
		it->second--;

		// 待機時間が終わったら削除
		if (it->second <= 0) {
			it = graveyard_.erase(it);
		} else {
			++it;
		}
	}

	// 鉱石の更新処理
	for (auto it = oreItems_.begin(); it != oreItems_.end(); ) {
		auto& ore = it->second;

		// 更新処理
		ore->Update();

		if (ore->IsDead()) {
			// 5フレーム後に削除するようにリストに追加
			graveyard_.push_back({ std::move(ore), 5 });

			// マップからは要素を削除
			it = oreItems_.erase(it);
		} else {
			it++;
		}
	}

	// 選択されていればアウトラインを更新
	if (selectId_ != -1 && oreItems_.size() > 0) {

		auto it = oreItems_.find(selectId_);
		if (it == oreItems_.end()) {
			return;
		}
		// 選択した鉱石の位置を取得
		OreItem* ore = GetOreItemForId();
		oreOutLineObject_->transform_.position = ore->GetPos();
		oreOutLineObject_->Update();
	}
}

void OreItemManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 鉱石を描画
	for (auto& [id, ore] : oreItems_) {
		ore->Draw(window, vpMatrix);
	}

	// アウトラインを描画
	if (selectId_ != -1 && oreItems_.size() > 0) {
		oreOutLineObject_->Draw(window, vpMatrix);
	}
}

void OreItemManager::DrawUI() {
#ifdef USE_IMGUI

	ImGui::Begin("OreItemInfo");
	// リストの情報を表示
	ImGui::Text("Total OreItems: %d", static_cast<int>(oreItems_.size()));
	ImGui::Separator();

	for (auto& [id, ore] : oreItems_) {
		ImGui::PushID(id);

		if (ImGui::TreeNode("OreNode", "Ore ID: %d", id)) {
			// HP
			int hp = ore->GetHp();
			int maxHp = ore->GetMaxHp();
			ImGui::Text("Hp: %d / %d", hp, maxHp);

			// 労働者数
			int currentWorker = ore->GetCurrentWorkerNum();
			int maxWorker = ore->GetMaxWorkerNum();
			ImGui::Text("Workers: %d / %d", currentWorker, maxWorker);

			// 座標
			Vector3 pos = ore->GetPos();
			float p[3] = { pos.x, pos.y, pos.z };
			ImGui::InputFloat3("Position", p, "%.2f", ImGuiInputTextFlags_ReadOnly);

			// サイズ
			Vector3 size = ore->GetSize();
			float s[3] = { size.x, size.y, size.z };
			ImGui::InputFloat3("Size", s, "%.2f", ImGuiInputTextFlags_ReadOnly);

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	ImGui::Separator();

	bool is = true;
	auto it = oreItems_.find(selectId_);
	if (it == oreItems_.end()) {
		is = false;
	}

	if (is) {
		OreItem* ore = GetOreItemForId();
		// HP
		int hp = ore->GetHp();
		int maxHp = ore->GetMaxHp();
		ImGui::Text("Hp: %d / %d", hp, maxHp);

		// 労働者数
		int currentWorker = ore->GetCurrentWorkerNum();
		int maxWorker = ore->GetMaxWorkerNum();
		ImGui::Text("Workers: %d / %d", currentWorker, maxWorker);

		// 座標
		Vector3 pos = ore->GetPos();
		float p[3] = { pos.x, pos.y, pos.z };
		ImGui::InputFloat3("Position", p, "%.2f", ImGuiInputTextFlags_ReadOnly);

		// サイズ
		Vector3 size = ore->GetSize();
		float s[3] = { size.x, size.y, size.z };
		ImGui::InputFloat3("Size", s, "%.2f", ImGuiInputTextFlags_ReadOnly);
	}
	ImGui::End();
#endif
}

void OreItemManager::AddOreItem(OreType type, const Vector3& pos) {

	int32_t index = currentId_++;

	switch (type)
	{
	case OreType::Large: {
		// 金鉱石を追加
		std::unique_ptr<GoldOre> ore = std::make_unique<GoldOre>();
		ore->Init(goldOreDrawData_, oreTexture_, pos, type);

		oreItems_[index] = std::move(ore);
		break;
	}

	case OreType::Medium: {
		// 金鉱石を追加
		std::unique_ptr<GoldOre> ore = std::make_unique<GoldOre>();
		ore->Init(goldOreDrawData_, oreTexture_, pos, type);

		oreItems_[index] = std::move(ore);
		break;
	}

	case OreType::Small: {
		// 金鉱石を追加
		std::unique_ptr<GoldOre> ore = std::make_unique<GoldOre>();
		ore->Init(goldOreDrawData_, oreTexture_, pos, type);

		oreItems_[index] = std::move(ore);
		break;
	}
	}
}

bool OreItemManager::IsSelectOre(const Vector3 selectpos, Vector3& worldPos) {

	// 選択した位置に鉱石が存在しているかを確認
	for (auto& [id, ore] : oreItems_) {

		Vector3 orePos = ore->GetPos();
		Vector3 oreSize = ore->GetSize();

		if (selectpos.x >= orePos.x - oreSize.x * 0.5f && selectpos.x <= orePos.x + oreSize.x * 0.5f) {
			if (selectpos.z >= orePos.z - oreSize.z * 0.5f && selectpos.z <= orePos.z + oreSize.z * 0.5f) {
				// 移動する位置を設定
				worldPos = orePos;
				selectId_ = id;
				return true;
			}
		}
	}

	// 何もない場合
	selectId_ = -1;
	return false;
}

OreItem* OreItemManager::GetOreItemForId() {
	// 選択した
	return oreItems_[selectId_].get();
}