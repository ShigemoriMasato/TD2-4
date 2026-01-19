#include"UnitManager.h"

void UnitManager::Initalize(MapChipField* mapChipField, DrawData playerDrawData, DrawData oreDrawData, KeyManager* keyManager) {
	// マップデータを取得
	mapChipField_ = mapChipField;

	// ユニットの出現位置を取得する
	homePosList_ = mapChipField_->GetHomePosList();

	// プレイヤーユニットを初期化
	playerUnit_ = std::make_unique<PlayerUnit>();
	playerUnit_->Initialize(mapChipField_, playerDrawData,{3.0f,0.0f,3.0f}, keyManager);

	// おれのモデルデータを取得
	oreDrawData_ = oreDrawData;

	// おれのメモリを確保
	oreUnits_.reserve(maxOreCount_);

}

void UnitManager::Update() {

	// プレイヤーの更新処理
	playerUnit_->Update();

	// おれユニットを削除
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

	// おれユニットの更新処理
	for (auto it = oreUnits_.begin(); it != oreUnits_.end(); ) {
		auto& unit = it->second;

		if (unit->IsDead()) {
			// 10フレーム後に削除するようにリストに追加
			graveyard_.push_back({ std::move(unit), 10 });

			// 再利用リストに追加
			freeIndices_.push_back(it->first);

			// マップからは要素を削除
			it = oreUnits_.erase(it);
		} else {
			// 生きている場合は更新
			unit->Update();
			++it;
		}
	}
}

void UnitManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// プレイヤーを描画
	playerUnit_->Draw(window, vpMatrix);

	// おれを描画
	for (const auto& [id, unit] : oreUnits_) {
		unit->Draw(window, vpMatrix);
	}
}

void UnitManager::AddOreUnit(const Vector3& targetPos) {

	// 出現位置を求める
	Vector3 homePos = GetNearHomePos(targetPos);

	int32_t index;
	if (!freeIndices_.empty()) {
		index = freeIndices_.front();
		freeIndices_.pop_front();
	} else {
		if (currentId_ >= maxOreCount_) {
			return;
		}
		index = currentId_++;
	}

	// 登録
	std::unique_ptr<OreUnit> oreUnit = std::make_unique<OreUnit>();
	oreUnit->Initialize(mapChipField_, oreDrawData_, homePos,targetPos,playerUnit_->GetPos());

	oreUnits_[index] = std::move(oreUnit);
}

Vector3 UnitManager::GetNearHomePos(const Vector3& targetPos) {

	if (homePosList_.empty()) {
		assert(0 && "Not HomePosList");
	}

	float minDis = FLT_MAX;
	Vector3 nearPos = homePosList_[0];

	for (const auto& pos : homePosList_) {
		float d = DistanceXZ(pos, targetPos);

		if (d < minDis) {
			minDis = d;
			nearPos = pos;
		}
	}

	return nearPos;
}