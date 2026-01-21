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
			// 10フレーム後に削除するようにリストに追加
			graveyard_.push_back({ std::move(ore), 10 });
		}
	}
}

void OreItemManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 鉱石を描画
	for (auto& [id, ore] : oreItems_) {
		ore->Draw(window, vpMatrix);
	}
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
				return true;
			}
		}
	}
	return false;
}