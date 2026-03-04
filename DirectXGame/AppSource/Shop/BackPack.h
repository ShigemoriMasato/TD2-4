#pragma once
#include <GameObject/DrawInfo.h>
#include <vector>

enum class Slot {
	Locked,
	Empty,
	Rank1,
	Rank2,
	Rank3, 
	Rank4,

	Count
};

class BackPack {
public:

	void Initialize();
	void AddSlot(std::pair<int, int> pos);
	Slot GetSlot(std::pair<int, int> pos) const;
	void SetSlot(std::pair<int, int> pos, Slot slot);
	Vector3 GetWorldPos(std::pair<int, int> pos) const;

	std::vector<DrawInfo> GetSlotDrawInfos() const;

private:

	std::vector<std::vector<Slot>> slots_;
	
	// スロットの描画原点位置
	Vector3 originPos_ = { 0.0f, 0.0f, 0.0f };

};
