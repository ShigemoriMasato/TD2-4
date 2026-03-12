#include "Piece.h"
#include "PieceManager.h"

void Piece::Initialize(const Item& item, int rank) {
	itemData_ = item;
	rank_ = rank;
	chips_.clear();
	chips_.resize(item.mapData.size());
	Vector3 maxPos = { 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i < item.mapData.size(); i++) {
		chips_[i] = item.mapData[i];
		maxPos.x = (float)std::max(int(maxPos.x), chips_[i].first);
		maxPos.z = (float)std::max(int(maxPos.z), chips_[i].second);
	}
	middleLocalPos_ = Vector3(maxPos.x * 0.5f, 0.0f, maxPos.z * 0.5f);
}

bool Piece::CanPut(BackPack* backPack)  {
	for (const auto& chip : chips_) {
		std::pair<int, int> slotPos = GetChipPos(chip);
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
		auto slot = GetChipPos(chip);
		backPack->SetSlot(slot, Slot::Rank1);
	}
	
	pieceManager_->MoveShopToHold(this);

	isPlaced_ = false;

	return true;
}

void Piece::Remove(BackPack* backPack) {
	for (const auto& chip : chips_) {
		auto slot = GetChipPos(chip);
		backPack->SetSlot(slot, Slot::Empty);
	}
}

void Piece::Use() {
	isUsing_ = true;
}

void Piece::SetPosition(const Vector3& pos) {
	//愚かしいことにもワールドポジションが送られてくるため、マップチップ番号に変換してから入力する
	Vector3 mappedPos = { std::roundf(pos.x), std::roundf(pos.y) , std::roundf(pos.z) };
	position_ = Vector3(std::round(mappedPos.x), 0.0f, std::round(mappedPos.z));
}

bool Piece::IsHovered(const Vector3& cursorPos, BackPack* backPack)  {
	for (const auto& chip : chips_) {
		std::pair<int, int> slotPos = GetChipPos(chip);
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
	for (const auto& chip : chips_) {
		DrawInfo info;
		auto slotPos = GetChipPos(chip);
		info.position = { (float)slotPos.first + 0.5f, 0.0f, (float)slotPos.second + 0.5f };
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
	DrawInfo info;
	info.modelIndex = itemData_.modelID;
	info.position = middleLocalPos_ + Vector3(itemData_.visualOffsetCells.x, 0.0f, itemData_.visualOffsetCells.y);
	switch (direction_) {
		case Direction::Up:
			break;
		case Direction::Right:
			info.position = Vector3(info.position.z, info.position.y, -info.position.x);
			break;
		case Direction::Down:
			info.position = Vector3(-info.position.x, info.position.y, -info.position.z);
			break;
		case Direction::Left:
			info.position = Vector3(-info.position.z, info.position.y, info.position.x);
			break;
	}
	info.position += Vector3(0.5f, 0.0f, 0.5f) + position_;
	info.scale = Vector3(0.5f, 0.5f, 0.5f);
	info.rotation.y = static_cast<float> (direction_) * 3.1415926535f * 0.5f;
	info.color = 0xffffffff;
	drawInfos.push_back(info);

	return drawInfos;
}

void Piece::RotateRight() {
	direction_ = static_cast<Direction>((static_cast<int>(direction_) + 1) % 4);
}

void Piece::RotateLeft() {
	direction_ = static_cast<Direction>((static_cast<int>(direction_) + 3) % 4);
}

std::pair<int, int> Piece::GetChipPos(const std::pair<int, int>& chip) const {
	std::pair<int, int> world = chip;
	int tmp;
	//回転処理
	switch (direction_) {
	case Direction::Up:
		break;
	case Direction::Right:
		tmp = world.first;
		world.first = world.second;
		world.second = tmp * -1;
		break;
	case Direction::Down:
		world.first *= -1;
		world.second *= -1;
		break;
	case Direction::Left:
		tmp = world.first;
		world.first = world.second * -1;
		world.second = tmp;
		break;
	}
	world.first += static_cast<int>(position_.x);
	world.second += static_cast<int>(position_.z);
	return world;
}

