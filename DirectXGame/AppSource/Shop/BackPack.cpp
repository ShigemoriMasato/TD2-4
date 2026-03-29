#include "BackPack.h"

void BackPack::Initialize() {
	// 指定サイズのスロットを初期化（保留エリアも含めて拡張）
	int totalHeight = height_ + reserveAreaHeight_ + 2; // 通常エリア + 保留エリア + 間隔
	slots_.resize(totalHeight, std::vector<Slot>(width_, Slot::Locked));

	// 初期領域をEmptyにする
	for (int i = emptyStartY_; i < emptyStartY_ + emptyAreaHeight_; ++i) {
		for (int j = emptyStartX_; j < emptyStartX_ + emptyAreaWidth_; ++j) {
			slots_[i][j] = Slot::Empty;
		}
	}

	// 保留エリアをEmptyにする
	for (int i = reserveStartY_; i < reserveStartY_ + reserveAreaHeight_; ++i) {
		for (int j = reserveStartX_; j < reserveStartX_ + reserveAreaWidth_; ++j) {
			if (i < static_cast<int>(slots_.size()) && j < static_cast<int>(slots_[0].size())) {
				slots_[i][j] = Slot::Empty;
			}
		}
	}
}

void BackPack::AddSlot(std::pair<int, int> localPos) {
	std::pair<int, int> pos = {
		static_cast<int>(static_cast<float>(localPos.first) + originPos_.x),
		static_cast<int>(static_cast<float>(localPos.second) + originPos_.z)
	};

	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return; // 範囲外の位置は無視
	}

	slots_[pos.second][pos.first] = Slot::Empty;
}

Slot BackPack::GetSlot(std::pair<int, int> localPos) const {
	std::pair<int, int> pos = { 
		static_cast<int>(static_cast<float>(localPos.first) - originPos_.x), 
		static_cast<int>(static_cast<float>(localPos.second) - originPos_.z) 
	};

	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return Slot::Unknown; // 範囲外の位置はUnknownを返す
	}

	return slots_[pos.second][pos.first];
}

void BackPack::SetSlot(std::pair<int, int> localPos, Slot slot) {
	std::pair<int, int> pos = {
		static_cast<int>(static_cast<float>(localPos.first) - originPos_.x),
		static_cast<int>(static_cast<float>(localPos.second) - originPos_.z)
	};

	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return; // 範囲外の位置は無視
	}

	if (slots_[pos.second][pos.first] != Slot::Locked) {
		slots_[pos.second][pos.first] = slot;
	}
}

void BackPack::UnlockSlot(std::pair<int, int> pos) {
	std::pair<int, int> localPos = {
		static_cast<int>(static_cast<float>(pos.first) - originPos_.x),
		static_cast<int>(static_cast<float>(pos.second) - originPos_.z)
	};

	if(localPos.first < 0 || localPos.first >= static_cast<int>(slots_[0].size()) ||
	   localPos.second < 0 || localPos.second >= static_cast<int>(slots_.size())) {
		return; // 範囲外の位置は無視
	}

	slots_[localPos.second][localPos.first] = Slot::Empty;
}

bool BackPack::IsInReserveArea(std::pair<int, int> localPos) const {
	std::pair<int, int> pos = {
		static_cast<int>(static_cast<float>(localPos.first) - originPos_.x),
		static_cast<int>(static_cast<float>(localPos.second) - originPos_.z)
	};

	return pos.first >= reserveStartX_ && pos.first < reserveStartX_ + reserveAreaWidth_ &&
		   pos.second >= reserveStartY_ && pos.second < reserveStartY_ + reserveAreaHeight_;
}

Vector3 BackPack::GetReserveAreaWorldPos() const {
	return originPos_ + Vector3(
		static_cast<float>(reserveStartX_) + static_cast<float>(reserveAreaWidth_) * 0.5f,
		0.0f,
		static_cast<float>(reserveStartY_) + static_cast<float>(reserveAreaHeight_) * 0.5f
	);
}

Vector3 BackPack::GetWorldPos(std::pair<int, int> pos) const {
	return Vector3(static_cast<float>(pos.first) + 0.5f, 0.0f, static_cast<float>(pos.second) + 0.5f);
}

std::vector<DrawInfo> BackPack::GetSlotDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	for (size_t i = 0; i < slots_.size(); ++i) {
		for (size_t j = 0; j < slots_[i].size(); ++j) {
			DrawInfo info;
			info.position = originPos_ + Vector3(static_cast<float>(j) + 0.5f, 0.0f, static_cast<float>(i) + 0.5f);
			info.scale = Vector3(0.5f, 0.1f, 0.5f);
			info.modelIndex = pieceModelID_;

			// 保留エリアかどうかをチェック
			bool isReserveArea = (static_cast<int>(i) >= reserveStartY_ && static_cast<int>(i) < reserveStartY_ + reserveAreaHeight_ &&
								  static_cast<int>(j) >= reserveStartX_ && static_cast<int>(j) < reserveStartX_ + reserveAreaWidth_);

			switch (slots_[i][j]) {
				case Slot::Locked:
					info.color = 0x555555FF; // グレー
					break;
				case Slot::Empty:
					if (isReserveArea) {
						info.color = 0x0080FFFF; // 青（保留エリア）
					} else {
						info.color = 0x00FF00FF; // 緑（通常エリア）
					}
					break;
				case Slot::Rank1:
					info.color = 0xFFFFFFFF; // 白
					break;
				default:
					info.color = 0xff;//黒
					break;
			}
			drawInfos.push_back(info);
		}
	}
	return drawInfos;
}
