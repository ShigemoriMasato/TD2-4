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

	std::vector<DrawInfo> GetSlotDrawInfos() const;
	
	void SetPieceModelID(int modelID) { pieceModelID_ = modelID; }

	void SetSize(int width, int height) { width_ = width; height_ = height; }
	void SetInitialEmptyArea(int startX, int startY, int width, int height) {
		emptyStartX_ = startX;
		emptyStartY_ = startY;
		emptyAreaWidth_ = width;
		emptyAreaHeight_ = height;
	}

	// 保留エリア関連
	void SetReserveArea(int startX, int startY, int width, int height) {
		reserveStartX_ = startX;
		reserveStartY_ = startY;
		reserveAreaWidth_ = width;
		reserveAreaHeight_ = height;
	}
	bool IsInReserveArea(std::pair<int, int> pos) const;
	Vector3 GetReserveAreaWorldPos() const;

	// 通常エリア（非保留エリア）の範囲を取得
	int GetNormalAreaStartX() const { return emptyStartX_; }
	int GetNormalAreaStartY() const { return emptyStartY_; }
	int GetNormalAreaWidth() const { return emptyAreaWidth_; }
	int GetNormalAreaHeight() const { return emptyAreaHeight_; }

	// 保留エリアの範囲を取得
	int GetReserveAreaStartX() const { return reserveStartX_; }
	int GetReserveAreaStartY() const { return reserveStartY_; }
	int GetReserveAreaWidth() const { return reserveAreaWidth_; }
	int GetReserveAreaHeight() const { return reserveAreaHeight_; }

	Vector3 GetOriginPos() const { return originPos_; }

private:

	std::vector<std::vector<Slot>> slots_;
	
	// スロットの描画原点位置
	Vector3 originPos_ = { 0.0f, 0.0f, -5.0f };

	int width_ = 10;
	int height_ = 8;

	// 初期の空き領域
	int emptyStartX_ = 1;
	int emptyStartY_ = 1;
	int emptyAreaWidth_ = 8;
	int emptyAreaHeight_ = 8;

	// 保留エリア（BackPackの下側)
	int reserveStartX_ = 0;
	int reserveStartY_ = 0;
	int reserveAreaWidth_ = 0;
	int reserveAreaHeight_ = 0;
	
	int pieceModelID_ = 0;

};
