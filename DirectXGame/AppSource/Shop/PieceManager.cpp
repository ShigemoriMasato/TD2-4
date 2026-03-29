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

std::vector<Piece*> PieceManager::Update(BackPack* backPack, float deltaTime) {
	std::vector<Piece*> usedPieces;
	for (const auto& piece : holdPieces_) {
		// Piece::Update内で保留中のPieceはスキップされる
		if (piece->Update(backPack, deltaTime)) {
			usedPieces.push_back(piece.get());
		}
	}
	allPieces_.erase(std::remove_if(allPieces_.begin(), allPieces_.end(),
		[](Piece* piece) { return !piece->IsActive(); }),
		allPieces_.end());
	holdPieces_.erase(std::remove_if(holdPieces_.begin(), holdPieces_.end(),
		[](const std::unique_ptr<Piece>& piece) { return !piece->IsActive(); }),
		holdPieces_.end());
	return usedPieces;
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
	bool isFromShop = false;
	for (size_t i = 0; i < shopPieces_.size(); ++i) {
		if (shopPieces_[i].get() == piece) {
			holdPieces_.push_back(std::move(shopPieces_[i]));
			shopPieces_.erase(shopPieces_.begin() + i);
			isFromShop = true;
			break;
		}
	}

	if (isFromShop) {
		// Remove remaining pieces from allPieces_
		for (auto it = allPieces_.begin(); it != allPieces_.end(); ) {
			bool isShopPiece = false;
			for (const auto& remainingPiece : shopPieces_) {
				if (*it == remainingPiece.get()) {
					isShopPiece = true;
					break;
				}
			}
			if (isShopPiece) {
				it = allPieces_.erase(it);
			} else {
				++it;
			}
		}
		// Clear the remaining shop pieces
		shopPieces_.clear();
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

bool PieceManager::IsShopPiece(Piece* piece) const {
	for (const auto& shopPiece : shopPieces_) {
		if (shopPiece.get() == piece) {
			return true;
		}
	}
	return false;
}
