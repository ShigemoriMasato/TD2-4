#include "Piece.h"
#include "PieceManager.h"
#include "Utility/Easing.h"
#include <../Engine/Assets/Audio/AudioManager.h>
#include <set>

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
	ignores_.reserve(chips_.size());
}

bool Piece::Update(BackPack* backPack, float deltaTime) {
	// 保留エリアに置かれている場合はUpdateしない
	if (isReserved_) {
		return false;
	}

	// 左クリックで持たれている場合はUpdateしない
	if (isHeld_) {
		return false;
	}

	// 武器は自動で使用状態にする
	if (itemData_.category == Category::Weapon && !isUsing_) {
		Use();
	}

	if (!isUsing_) {
		return false;
	}

	useTimer_ += deltaTime;

	// 使用時間が一定時間を超えたら使用終了
	if (useTimer_ >= deleteTime_) {
		isUsing_ = false;

		// 使用が終わったらチップをひとつバックパックから外す
		backPack->SetSlot(GetChipPos(chips_[ignores_.size()]), Slot::Empty);
		ignores_.push_back(chips_[ignores_.size()]);
		if (chips_.size() == ignores_.size()) {
			isActive_ = false;
			return false;
		}

		useTimer_ = 0.0f;
	}

	return true;
}

bool Piece::CanPut(BackPack* backPack) {
	for (const auto& chip : chips_) {
		if (IsIgnored(chip)) {
			continue;
		}
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
		if(IsIgnored(chip)) {
			continue;
		}
		auto slot = GetChipPos(chip);
		backPack->SetSlot(slot, Slot::Rank1);
	}

	pieceManager_->MoveShopToHold(this);

	// 保留エリアに置かれたかをチェック
	bool inReserveArea = true;
	for (const auto& chip : chips_) {
		if (IsIgnored(chip)) {
			continue;
		}
		auto slot = GetChipPos(chip);
		if (!backPack->IsInReserveArea(slot)) {
			inReserveArea = false;
			break;
		}
	}
	isReserved_ = inReserveArea;

	isPlaced_ = false;

	return true;
}

void Piece::Remove(BackPack* backPack) {
	for (const auto& chip : chips_) {
		if (IsIgnored(chip)) {
			continue;
		}
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

Vector3 Piece::GetCenterOffset() const {
	Vector3 offset = middleLocalPos_;
	Vector3 rotatedOffset = offset;
	switch (direction_) {
	case Direction::Up:
		rotatedOffset = offset;
		break;
	case Direction::Right:
		rotatedOffset = Vector3(offset.z, offset.y, -offset.x);
		break;
	case Direction::Down:
		rotatedOffset = Vector3(-offset.x, offset.y, -offset.z);
		break;
	case Direction::Left:
		rotatedOffset = Vector3(-offset.z, offset.y, offset.x);
		break;
	}
	return rotatedOffset;
}

bool Piece::IsHovered(const Vector3& cursorPos, BackPack* backPack) {
	for (const auto& chip : chips_) {
		if (IsIgnored(chip)) {
			continue;
		}
		std::pair<int, int> slotPos = GetChipPos(chip);
		Vector3 slotWorldPos = backPack->GetWorldPos(slotPos);
		if (std::abs((cursorPos.x + hoverOffsetX) - slotWorldPos.x) < hoverSizeX &&
			std::abs((cursorPos.z + hoverOffsetZ) - slotWorldPos.z) < hoverSizeZ) {

			if(!isHovered_){
				AudioManager::GetInstance()->GetData("ItemSelect.mp3")->Play();
			}

			isHovered_ = true;
			return true;
		}
	}
	isHovered_ = false;
	return false;
}

std::vector<DrawInfo> Piece::GetDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	int totalChips = static_cast<int>(chips_.size());
	int currentIdx = 0;

	float deleteT = 1.0f - (useTimer_ / deleteTime_ * 0.7f) + 0.3f;

	// チップの位置を集合として保持（アウトライン判定用）
	std::set<std::pair<int, int>> chipSet;
	for (const auto& chip : chips_) {
		if (!IsIgnored(chip)) {
			chipSet.insert(GetChipPos(chip));
		}
	}

	for (const auto& chip : chips_) {
		if(IsIgnored(chip)) {
			currentIdx++;
			continue;
		}
		DrawInfo info;
		auto slotPos = GetChipPos(chip);
		info.position = { (float)slotPos.first + 0.5f, 0.0f, (float)slotPos.second + 0.5f };
		info.scale = Vector3(0.5f, 0.1f, 0.5f);
		info.modelIndex = pieceModelID;

		info.color = 0x4f4f4fff;
		if (isPlaced_) {
			info.color = 0x00ffffff; // シアン
		}

		drawInfos.push_back(info);
		currentIdx++;
	}

	// アウトライン描画（各チップの境界に白い線を追加）
	const float outlineThickness = 0.05f; // アウトラインの太さ
	const float outlineHeight = 0.15f; // アウトラインの高さ（チップより少し高く）
	const uint32_t outlineColor = isHovered_ ? 0xffff00ff : 0xFFFFFFFF; // ホバー時は黄色、通常は白

	for (const auto& chip : chips_) {
		if (IsIgnored(chip)) {
			continue;
		}
		auto slotPos = GetChipPos(chip);

		// 4方向（上下左右）をチェック
		std::pair<int, int> directions[4] = {
			{0, -1},  // 上
			{0, 1},   // 下
			{-1, 0},  // 左
			{1, 0}    // 右
		};

		for (int dir = 0; dir < 4; ++dir) {
			std::pair<int, int> neighbor = {
				slotPos.first + directions[dir].first,
				slotPos.second + directions[dir].second
			};

			// 隣接位置にチップがない場合、その辺にアウトラインを描画
			if (chipSet.find(neighbor) == chipSet.end()) {
				DrawInfo outlineInfo;
				outlineInfo.position = { (float)slotPos.first + 0.5f, 0.0f, (float)slotPos.second + 0.5f };
				outlineInfo.modelIndex = pieceModelID;
				outlineInfo.color = outlineColor;

				// 方向に応じてアウトラインの位置とスケールを調整
				if (dir == 0) { // 上
					outlineInfo.position.z -= 0.5f;
					outlineInfo.scale = Vector3(0.5f, outlineHeight, outlineThickness);
				} else if (dir == 1) { // 下
					outlineInfo.position.z += 0.5f;
					outlineInfo.scale = Vector3(0.5f, outlineHeight, outlineThickness);
				} else if (dir == 2) { // 左
					outlineInfo.position.x -= 0.5f;
					outlineInfo.scale = Vector3(outlineThickness, outlineHeight, 0.5f);
				} else { // 右
					outlineInfo.position.x += 0.5f;
					outlineInfo.scale = Vector3(outlineThickness, outlineHeight, 0.5f);
				}

				drawInfos.push_back(outlineInfo);
			}
		}
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

	float scaleLerpT = 0.0f;
	if (deleteTime_ > 0.0f) {
		scaleLerpT = std::clamp(useTimer_ / deleteTime_, 0.0f, 1.0f);
	}
	info.scale = lerp(weaponStartScale_, weaponEndScale_, scaleLerpT, EaseType::Linear);

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

bool Piece::IsIgnored(const std::pair<int, int>& chip) const {
	return std::find_if(ignores_.begin(), ignores_.end(), [&chip](const std::pair<int, int>& c) { return c == chip; }) != ignores_.end();
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

bool Piece::AutoPlace(BackPack* backPack) {
	// 通常エリアに配置を試みる
	int normalStartX = backPack->GetNormalAreaStartX();
	int normalStartY = backPack->GetNormalAreaStartY();
	int normalWidth = backPack->GetNormalAreaWidth();
	int normalHeight = backPack->GetNormalAreaHeight();
	Vector3 originPos = backPack->GetOriginPos();

	// 通常エリア内の全ての位置を試す（回転も含む）
	for (int rotation = 0; rotation < 4; ++rotation) {
		for (int y = normalStartY; y < normalStartY + normalHeight; ++y) {
			for (int x = normalStartX; x < normalStartX + normalWidth; ++x) {
				// ワールド座標に変換
				Vector3 worldPos = originPos + Vector3(static_cast<float>(x), 0.0f, static_cast<float>(y));
				SetPosition(worldPos - GetCenterOffset());
				
				if (CanPut(backPack)) {
					return Put(backPack);
				}
			}
		}
		// 次の回転を試す
		RotateRight();
	}

	// 通常エリアに配置できなかった場合、保留エリアに配置を試みる
	int reserveStartX = backPack->GetReserveAreaStartX();
	int reserveStartY = backPack->GetReserveAreaStartY();
	int reserveWidth = backPack->GetReserveAreaWidth();
	int reserveHeight = backPack->GetReserveAreaHeight();

	for (int rotation = 0; rotation < 4; ++rotation) {
		for (int y = reserveStartY; y < reserveStartY + reserveHeight; ++y) {
			for (int x = reserveStartX; x < reserveStartX + reserveWidth; ++x) {
				// ワールド座標に変換
				Vector3 worldPos = originPos + Vector3(static_cast<float>(x), 0.0f, static_cast<float>(y));
				SetPosition(worldPos - GetCenterOffset());
				
				if (CanPut(backPack)) {
					return Put(backPack);
				}
			}
		}
		// 次の回転を試す
		RotateRight();
	}

	// どこにも配置できなかった
	return false;
}

bool Piece::MoveToReserve(BackPack* backPack) {
	// すでに保留エリアにある場合は何もしない
	if (isReserved_) {
		return false;
	}

	// 元の位置と回転を保存
	Vector3 originalPos = position_;
	Direction originalDir = direction_;

	// 現在の位置から削除
	Remove(backPack);

	// 保留エリアに配置を試みる
	int reserveStartX = backPack->GetReserveAreaStartX();
	int reserveStartY = backPack->GetReserveAreaStartY();
	int reserveWidth = backPack->GetReserveAreaWidth();
	int reserveHeight = backPack->GetReserveAreaHeight();
	Vector3 originPos = backPack->GetOriginPos();

	for (int rotation = 0; rotation < 4; ++rotation) {
		for (int y = reserveStartY; y < reserveStartY + reserveHeight; ++y) {
			for (int x = reserveStartX; x < reserveStartX + reserveWidth; ++x) {
				// ワールド座標に変換
				Vector3 worldPos = originPos + Vector3(static_cast<float>(x), 0.0f, static_cast<float>(y));
				SetPosition(worldPos - GetCenterOffset());
				
				if (CanPut(backPack)) {
					return Put(backPack);
				}
			}
		}
		// 次の回転を試す
		RotateRight();
	}

	// 保留エリアに配置できなかった場合、元の位置に戻す
	// 回転を元に戻す
	while (direction_ != originalDir) {
		RotateRight();
	}
	SetPosition(originalPos);
	
	// 元の位置に戻す
	if (CanPut(backPack)) {
		Put(backPack);
	}

	return false;
}

bool Piece::MoveToNormal(BackPack* backPack) {
	// 保留エリアにない場合は何もしない
	if (!isReserved_) {
		return false;
	}

	// 元の位置と回転を保存
	Vector3 originalPos = position_;
	Direction originalDir = direction_;

	// 現在の位置から削除
	Remove(backPack);

	// 通常エリアに配置を試みる
	int normalStartX = backPack->GetNormalAreaStartX();
	int normalStartY = backPack->GetNormalAreaStartY();
	int normalWidth = backPack->GetNormalAreaWidth();
	int normalHeight = backPack->GetNormalAreaHeight();
	Vector3 originPos = backPack->GetOriginPos();

	for (int rotation = 0; rotation < 4; ++rotation) {
		for (int y = normalStartY; y < normalStartY + normalHeight; ++y) {
			for (int x = normalStartX; x < normalStartX + normalWidth; ++x) {
				// ワールド座標に変換
				Vector3 worldPos = originPos + Vector3(static_cast<float>(x), 0.0f, static_cast<float>(y));
				SetPosition(worldPos - GetCenterOffset());
				
				if (CanPut(backPack)) {
					return Put(backPack);
				}
			}
		}
		// 次の回転を試す
		RotateRight();
	}

	// 通常エリアに配置できなかった場合、元の位置に戻す
	// 回転を元に戻す
	while (direction_ != originalDir) {
		RotateRight();
	}
	SetPosition(originalPos);
	
	// 元の位置に戻す
	if (CanPut(backPack)) {
		Put(backPack);
	}

	return false;
}

