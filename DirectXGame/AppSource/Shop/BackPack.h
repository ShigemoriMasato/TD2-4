#pragma once
#include <GameObject/DrawInfo.h>
#include <vector>

enum class Slot {
	Unknown,
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
	void UnlockSlot(std::pair<int, int> pos);
	Vector3 GetWorldPos(std::pair<int, int> pos) const;

	Vector3 GetOriginPos() const { return originPos_; }

	std::vector<DrawInfo> GetSlotDrawInfos() const;

	void SetSize(int width, int height) { width_ = width; height_ = height; }
	void SetInitialEmptyArea(int startX, int startY, int width, int height) {
		emptyStartX_ = startX;
		emptyStartY_ = startY;
		emptyAreaWidth_ = width;
		emptyAreaHeight_ = height;
	}

private:

	std::vector<std::vector<Slot>> slots_;
	
	// スロットの描画原点位置
	Vector3 originPos_ = { 0.0f, 0.0f, -5.0f };

	int width_ = 10;
	int height_ = 10;

	// 初期の空き領域
	int emptyStartX_ = 2;
	int emptyStartY_ = 2;
	int emptyAreaWidth_ = 6;
	int emptyAreaHeight_ = 6;

};
