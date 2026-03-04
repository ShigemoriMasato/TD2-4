#include "BackPack.h"

void BackPack::Initialize() {
	// 5x5のスロットを初期化
	slots_.resize(10, std::vector<Slot>(10, Slot::Locked));

	// 中央の5x5をEmptyにする
	for (int i = 3; i < 7; ++i) {
		for (int j = 3; j < 7; ++j) {
			slots_[i][j] = Slot::Empty;
		}
	}
}

void BackPack::AddSlot(std::pair<int, int> pos) {
	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return; // 範囲外の位置は無視
	}

	slots_[pos.second][pos.first] = Slot::Empty;
}

Slot BackPack::GetSlot(std::pair<int, int> pos) const {
	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return Slot::Unknown; // 範囲外の位置はUnknownを返す
	}

	return slots_[pos.second][pos.first];
}

void BackPack::SetSlot(std::pair<int, int> pos, Slot slot) {
	if(pos.first < 0 || pos.first >= static_cast<int>(slots_[0].size()) ||
	   pos.second < 0 || pos.second >= static_cast<int>(slots_.size())) {
		return; // 範囲外の位置は無視
	}

	if (slots_[pos.second][pos.first] != Slot::Locked) {
		slots_[pos.second][pos.first] = slot;
	}
}

Vector3 BackPack::GetWorldPos(std::pair<int, int> pos) const {
	return originPos_ + Vector3(static_cast<float>(pos.first) + 0.5f, 0.0f, static_cast<float>(pos.second) + 0.5f);
}

std::vector<DrawInfo> BackPack::GetSlotDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	for (size_t i = 0; i < slots_.size(); ++i) {
		for (size_t j = 0; j < slots_[i].size(); ++j) {
			DrawInfo info;
			info.position = originPos_ + Vector3(static_cast<float>(j) + 0.5f, 0.0f, static_cast<float>(i) + 0.5f);
			info.scale = Vector3(1.0f, 0.2f, 1.0f);
			info.modelIndex = 0;
			switch (slots_[i][j]) {
				case Slot::Locked:
					info.color = 0x555555FF; // グレー
					break;
				case Slot::Empty:
					info.color = 0x00FF00FF; // 緑
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
