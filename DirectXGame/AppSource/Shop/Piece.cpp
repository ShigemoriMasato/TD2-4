#include "Piece.h"

void Piece::Initialize(Item& item) {
	itemData_ = item;
	chips_.clear();
	chips_.resize(item.mapData.size());
	for (size_t i = 0; i < item.mapData.size(); i++) {
		chips_[i] = item.mapData[i];
	}
	SetPosition({ 0.0f, 0.0f, 0.0f });
}

bool Piece::CanPut(BackPack* backPack) const {
	for (const auto& chip : chips_) {
		if (backPack->GetSlot(chip) != Slot::Empty) {
			return false;
		}
	}
	return true;
}

bool Piece::Put(BackPack* backPack) {
	if (!CanPut(backPack)) {
		return false;
	}

	for (const auto& chip : chips_) {
		backPack->SetSlot(chip, Slot::Rank1);
	}
}

bool Piece::IsHovered(const Vector3& cursorPos, BackPack* backPack) const {
	for (const auto& chip : chips_) {
		Vector3 slotWorldPos = backPack->GetWorldPos(chip);
		if (std::abs(cursorPos.x - slotWorldPos.x) < 0.5f &&
			std::abs(cursorPos.z - slotWorldPos.z) < 0.5f) {
			return true;
		}
	}
	return false;
}

