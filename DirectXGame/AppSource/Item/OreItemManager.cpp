#include"OreItemManager.h"

// 各鉱石
#include"Object/GoldOre.h"

void OreItemManager::Initialize(const DrawData& goldOreDrawData) {

	// 金鉱石の描画データを取得
	goldOreDrawData_ = goldOreDrawData;

	// メモリを確保
	oreItems_.reserve(10);

	// テストで鉱石を設定
	AddOreItem(OreType::Gold, {4.0f,0.0f,7.0f});
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
	for (auto& [id, ore] : oreItems_) {
		ore->Update();

		if (ore->IsDead()) {
			// 5フレーム後に削除するようにリストに追加
			graveyard_.push_back({ std::move(ore), 5 });
		}
	}
}

void OreItemManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 鉱石を描画
	for (auto& [id, ore] : oreItems_) {
		ore->Draw(window, vpMatrix);
	}

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
	ImGui::End();
}

void OreItemManager::AddOreItem(OreType type, const Vector3& pos) {

	int32_t index = currentId_++;

	switch (type)
	{
	case OreType::Gold:
		// 金鉱石を追加
		std::unique_ptr<GoldOre> ore = std::make_unique<GoldOre>();
		ore->Init(goldOreDrawData_, pos);

		oreItems_[index] = std::move(ore);
		break;
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
	return false;
}

OreItem* OreItemManager::GetOreItemForId() {
	// 選択した
	return oreItems_[selectId_].get();
}