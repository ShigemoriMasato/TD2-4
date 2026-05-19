#include "Shop.h"
#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif
#include <GameObject/Weapon/WeaponData.h>

namespace {
	// レアリティを重み付きランダムで選択する
	WeaponRarity RollRarity(std::mt19937& rng) {
		// Common:50% Uncommon:30% Rare:15% Epic:4% Legend:1%
		std::discrete_distribution<int> dist({ 90, 10, 0, 0, 0 });
		return static_cast<WeaponRarity>(dist(rng));
	}
}

void Shop::Initialize(ItemManager* itemManager) {
	itemManager_ = itemManager;
	auto items = itemManager->GetAllItems();
	itemIndices_.clear();

	//Itemのインデックスを作成しておく。いつかIDにする
	for(const auto& [id, item] : items) {
		if (item.isActive) {
			itemIndices_.push_back(id);
		}
	}
}

std::vector<std::unique_ptr<Piece>> Shop::RefreshShopPieces() {
	std::vector<std::unique_ptr<Piece>> shopPieces;
	// アイテムのインデックスからランダムに3つ選ぶ
	std::shuffle(itemIndices_.begin(), itemIndices_.end(), randomEngine_);
	int selectCount = 3;
	for (int i = 0; i < selectCount && i < static_cast<int>(itemIndices_.size()); ++i) {
		int itemIndex = itemIndices_[i];
		const Item& item = itemManager_->GetItem(itemIndex);
		auto piece = std::make_unique<Piece>();
		piece->Initialize(item, 0);
		piece->SetRarity(RollRarity(randomEngine_));

		//初期位置(適当)
		piece->SetPosition(startPos_ + interval_ * static_cast<float>(i));

		shopPieces.push_back(std::move(piece));
	}
	return shopPieces;
}

void Shop::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Shop");
	ImGui::DragFloat3("Start Pos", &startPos_.x, 0.1f);
	ImGui::DragFloat3("Interval", &interval_.x, 0.1f);
	ImGui::Text("Hover Detect");
	ImGui::DragFloat("Hover Size X", &Piece::hoverSizeX, 0.01f);
	ImGui::DragFloat("Hover Size Z", &Piece::hoverSizeZ, 0.01f);
	ImGui::DragFloat("Hover Offset X", &Piece::hoverOffsetX, 0.01f);
	ImGui::DragFloat("Hover Offset Z", &Piece::hoverOffsetZ, 0.01f);
	ImGui::End();
#endif
}
