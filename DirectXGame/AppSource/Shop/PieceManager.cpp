#include "PieceManager.h"

void PieceManager::Initialize(std::vector<std::unique_ptr<Piece>>& playerPieces) {
	holdPieces_ = std::move(playerPieces);
	for (const auto& piece : holdPieces_) {
		allPieces_.push_back(piece.get());
	}
}

void PieceManager::UpdateItemInfo(ItemManager* itemManager) {
	for (const auto& piece : shopPieces_) {
		piece->Initialize(itemManager->GetItem(piece->GetItem().id));
	}
	for (const auto& piece : holdPieces_) {
		piece->Initialize(itemManager->GetItem(piece->GetItem().id));
	}
}

void PieceManager::RefreshShopPieces(std::vector<std::unique_ptr<Piece>> shopPieces) {
	for (size_t i = 0; i < shopPieces_.size(); ++i) {
		for (size_t j = 0; j < allPieces_.size(); ++j) {
			if (allPieces_[j] == shopPieces_[i].get()) {
				allPieces_.erase(allPieces_.begin() + j);
				break;
			}
		}
	}

	shopPieces_ = std::move(shopPieces);
	for (const auto& piece : shopPieces_) {
		allPieces_.push_back(piece.get());
	}
}

void PieceManager::MoveShopToHold(Piece* piece) {
	for (size_t i = 0; i < shopPieces_.size(); ++i) {
		if(shopPieces_[i].get() == piece) {
			holdPieces_.push_back(std::move(shopPieces_[i]));
			shopPieces_.erase(shopPieces_.begin() + i);
			break;
		}
	}
}

std::vector<Piece*> PieceManager::GetAllPieces() {
	return allPieces_;
}

std::vector<std::unique_ptr<Piece>>&& PieceManager::GetHoldPieces() {
	return std::move(holdPieces_);
}
