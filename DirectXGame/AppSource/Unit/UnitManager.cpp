#include"UnitManager.h"

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

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

	// おれユニットの更新処理
	for (auto& [id, unit] : oreUnits_) {
		// 生きている場合は更新
		if (unit->IsActive() && !unit->IsDead()) {
			// ユニットの更新処理
			unit->Update();

			// 更新して有効フラグがfalseだったら、再利用リストに追加する
			if (!unit->IsActive()) {
				// 再利用リストに追加
				freeIndices_.push_back(id);
				activeCount_--;
			} else if (unit->IsDead()) {
				// 再利用リストに追加
				freeIndices_.push_back(id);
				activeCount_--;
				// 死亡していた場合、利用出来る最大数を減らす
				maxCurrentOreCount_--;
			}
		}
	}
}

void UnitManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// プレイヤーを描画
	playerUnit_->Draw(window, vpMatrix);

	// おれを描画
	for (const auto& [id, unit] : oreUnits_) {
		if (!unit->IsActive()) { continue; }
		unit->Draw(window, vpMatrix);
	}

#ifdef USE_IMGUI

	ImGui::Begin("OreUnitsInfo");
	ImGui::Text("MaxNumInstace : %d", maxOreCount_);
	ImGui::Text("MaxCurrentNum : %d", maxCurrentOreCount_);
	ImGui::Text("OreUnitInstace : %d", static_cast<int>(oreUnits_.size()));
	ImGui::Text("ActiveNum : %d", activeCount_);
	ImGui::End();
#endif
}

void UnitManager::AddOreUnit(const Vector3& targetPos) {

	// 生成出来る最大の数を超えていれば、早期リターン
	if (activeCount_ >= maxCurrentOreCount_) {
		return;
	}

	// 出現位置を求める
	Vector3 homePos = GetNearHomePos(targetPos);

	bool isReuse = false;
	int32_t index;
	if (!freeIndices_.empty()) {
		index = freeIndices_.front();
		freeIndices_.pop_front();
		isReuse = true;
	} else {
		if (currentId_ >= maxOreCount_ || oreUnits_.size() >= maxCurrentOreCount_) {
			return;
		}
		index = currentId_++;
	}

	if (isReuse) {
		// 既存のユニットを再利用
		auto unit = oreUnits_.find(index);
		if (unit == oreUnits_.end()) {
			assert(false && "Not found Unit");
		}
		// 初期化
		unit->second->Initialize(homePos, targetPos);
	} else {
		// 新しく登録
		std::unique_ptr<OreUnit> oreUnit = std::make_unique<OreUnit>(mapChipField_, oreDrawData_, playerUnit_->GetPos());
		oreUnit->Initialize(homePos, targetPos);

		oreUnits_[index] = std::move(oreUnit);
	}

	activeCount_++;
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