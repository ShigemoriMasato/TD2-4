#pragma once
#include <GameObject/Item/Item.h>
#include "BackPack.h"

class PieceManager;

class Piece {
public:

	enum class Direction {
		Up,
		Right,
		Down,
		Left,
	};


	Piece() = default;
	Piece(const Item& item, int rank) { Initialize(item, rank); }

	static void SetPieceManager(PieceManager* manager) { pieceManager_ = manager; }

	void Initialize(const Item& item, int rank);

	void SetPosition(const Vector3& pos);

	Vector3 GetPosition() const { return position_; }
	bool CanPut(BackPack* backPack) ;
	bool Put(BackPack* backPack);
	bool IsHovered(const Vector3& cursorPos, BackPack* backPack) ;
	std::vector<DrawInfo> GetDrawInfos() const;
	Item GetItem() const { return itemData_; }
	int GetRank() const { return rank_; }

	void RotateRight();
	void RotateLeft();
	Direction GetDirection() const { return direction_; }

private:

	std::pair<int, int> GetChipPos(const std::pair<int, int>& chip) const;

	static inline PieceManager* pieceManager_ = nullptr;

	Direction direction_ = Direction::Up;

	Item itemData_;
	int rank_ = 0;
	std::vector<std::pair<int, int>> chips_;

	Vector3 middleLocalPos_ = { 0.0f, 0.0f, 0.0f };
	Vector3 position_{};

	bool isHovered_ = false;
	bool isPlaced_ = false;

};
