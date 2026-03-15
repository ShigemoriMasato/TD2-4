#include "BackPack.h"

void BackPack::Initialize() {
	// 10x10のスロットを初期化
	slots_.resize(10, std::vector<Slot>(10, Slot::Locked));

	// 中央の高さ3、横幅5をEmptyにする
	for (int i = 3; i < 6; ++i) {
		for (int j = 2; j < 7; ++j) {
			slots_[i][j] = Slot::Empty;
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

Vector3 BackPack::GetWorldPos(std::pair<int, int> pos) const {
	return Vector3(static_cast<float>(pos.first) + 0.5f, 0.0f, static_cast<float>(pos.second) + 0.5f);
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
