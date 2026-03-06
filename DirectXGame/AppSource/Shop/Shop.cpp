#include "Shop.h"

void Shop::Initialize(ItemManager* itemManager) {
	itemManager_ = itemManager;
	auto items = itemManager->GetAllItems();

	//Itemのインデックスを作成しておく。いつかIDにする
	for (size_t i = 0; i < items.size(); ++i) {
		itemIndices_.push_back(static_cast<int>(i));
	}
}

std::vector<std::unique_ptr<Piece>> Shop::RefreshShopPieces() {
	std::vector<std::unique_ptr<Piece>> shopPieces;
	// アイテムのインデックスからランダムに3つ選ぶ
	std::shuffle(itemIndices_.begin(), itemIndices_.end(), randomEngine_);
	for (int i = 0; i < 3 && i < static_cast<int>(itemIndices_.size()); ++i) {
		int itemIndex = itemIndices_[i];
		const Item& item = itemManager_->GetItem(itemIndex);
		auto piece = std::make_unique<Piece>();
		piece->Initialize(item);

		//初期位置
		piece->SetPosition(Vector3(-4.0f, 0.0f, -4.0f + i * 4.0f));

		shopPieces.push_back(std::move(piece));
	}
	return shopPieces;
}
