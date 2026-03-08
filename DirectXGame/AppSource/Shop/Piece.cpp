#include "Piece.h"
#include "PieceManager.h"

void Piece::Initialize(const Item& item) {
	itemData_ = item;
	chips_.clear();
	chips_.resize(item.mapData.size());
	for (size_t i = 0; i < item.mapData.size(); i++) {
		chips_[i] = item.mapData[i];
	}
}

bool Piece::CanPut(BackPack* backPack)  {
	for (const auto& chip : chips_) {
		std::pair<int, int> slotPos = { static_cast<int>(position_.x) + chip.first, static_cast<int>(position_.z) + chip.second };
		if (backPack->GetSlot(slotPos) != Slot::Empty) {
			isPlaced_ = false;
			return false;
		}
	}
	isPlaced_ = true;
	return true;
}

bool Piece::Put(BackPack* backPack) {
	if (!CanPut(backPack)) {
		return false;
	}

	for (const auto& chip : chips_) {
		backPack->SetSlot(chip, Slot::Rank1);
	}
	
	pieceManager_->MoveShopToHold(this);

	isPlaced_ = false;

	return true;
}

void Piece::SetPosition(const Vector3& pos) {
	//愚かしいことにもワールドポジションが送られてくるため、マップチップ番号に変換してから入力する
	Vector3 mappedPos = pos - Vector3(0.5f, 0.0f, 0.5f);
	position_ = Vector3(std::round(mappedPos.x), 0.0f, std::round(mappedPos.z));
}

bool Piece::IsHovered(const Vector3& cursorPos, BackPack* backPack)  {
	for (const auto& chip : chips_) {
		std::pair<int, int> slotPos = { static_cast<int>(position_.x) + chip.first, static_cast<int>(position_.z) + chip.second };
		Vector3 slotWorldPos = backPack->GetWorldPos(slotPos);
		if (std::abs(cursorPos.x - slotWorldPos.x) < 0.5f &&
			std::abs(cursorPos.z - slotWorldPos.z) < 0.5f) {
			isHovered_ = true;
			return true;
		}
	}
	isHovered_ = false;
	return false;
}

std::vector<DrawInfo> Piece::GetDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	DrawInfo info;
	for (const auto& chip : chips_) {
		info.position = position_ + Vector3(static_cast<float>(chip.first) + 0.5f, 0.0f, static_cast<float>(chip.second) + 0.5f);
		info.scale = Vector3(1.0f, 0.2f, 1.0f);
		info.modelIndex = 0;

		info.color = 0x2020b0ff; // 青色
		if (isHovered_) {
			info.color = 0xffff00ff; // 黄色
		}
		if(isPlaced_) {
			info.color = 0x00ffffff; // シアン
		}

		drawInfos.push_back(info);
	}
	info.modelIndex = itemData_.modelID;
	info.position = position_ + Vector3(0.5f, 0.0f, 0.5f) + Vector3(itemData_.visualOffsetCells.x, 0.0f, itemData_.visualOffsetCells.y);
	info.scale = Vector3(0.5f, 0.5f, 0.5f);
	info.color = 0xffffffff;
	drawInfos.push_back(info);

	return drawInfos;
}

