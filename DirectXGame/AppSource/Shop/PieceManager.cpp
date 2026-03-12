#include "PieceManager.h"

void PieceManager::Initialize() {
	holdPieces_.clear();
	for (const auto& piece : holdPieces_) {
		allPieces_.push_back(piece.get());
	}
}

void PieceManager::UpdateItemInfo(ItemManager* itemManager) {
	for (const auto& piece : shopPieces_) {
		piece->Initialize(itemManager->GetItem(piece->GetItem().id), piece->GetRank());
	}
	for (const auto& piece : holdPieces_) {
		piece->Initialize(itemManager->GetItem(piece->GetItem().id), piece->GetRank());
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
		if (shopPieces_[i].get() == piece) {
			holdPieces_.push_back(std::move(shopPieces_[i]));
			shopPieces_.erase(shopPieces_.begin() + i);
			break;
		}
	}
}

void PieceManager::RemovePiece(Piece* piece) {
	for (size_t i = 0; i < allPieces_.size(); ++i) {
		if (allPieces_[i] == piece) {
			allPieces_.erase(allPieces_.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < shopPieces_.size(); ++i) {
		if (shopPieces_[i].get() == piece) {
			shopPieces_.erase(shopPieces_.begin() + i);
			return;
		}
	}

	for (size_t i = 0; i < holdPieces_.size(); ++i) {
		if (holdPieces_[i].get() == piece) {
			holdPieces_.erase(holdPieces_.begin() + i);
			return;
		}
	}
}

std::vector<Piece*> PieceManager::GetAllPieces() {
	return allPieces_;
}

void PieceManager::GetHoldPieces(std::vector<Piece*>& pieces) {
	pieces.clear();
	for (const auto& piece : holdPieces_) {
		pieces.push_back(piece.get());
	}
}
