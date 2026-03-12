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

	bool Update(BackPack* backPack, float deltaTime);

	void SetPosition(const Vector3& pos);

	Vector3 GetPosition() const { return position_; }
	bool CanPut(BackPack* backPack) ;
	bool Put(BackPack* backPack);
	void Remove(BackPack* backPack);
	void Use();

	bool IsHovered(const Vector3& cursorPos, BackPack* backPack) ;
	std::vector<DrawInfo> GetDrawInfos() const;
	Item GetItem() const { return itemData_; }
	int GetRank() const { return rank_; }
	bool IsActive() const { return isActive_; }

	void RotateRight();
	void RotateLeft();
	Direction GetDirection() const { return direction_; }

private:

	bool IsIgnored(const std::pair<int, int>& chip) const;
	std::pair<int, int> GetChipPos(const std::pair<int, int>& chip) const;

	static inline PieceManager* pieceManager_ = nullptr;

	Direction direction_ = Direction::Up;

	Item itemData_;
	int rank_ = 0;
	std::vector<std::pair<int, int>> chips_;
	std::vector<std::pair<int, int>> ignores_;

	Vector3 middleLocalPos_ = { 0.0f, 0.0f, 0.0f };
	Vector3 position_{};

	bool isHovered_ = false;
	bool isPlaced_ = false;

	bool isUsing_ = false;
	bool isActive_ = true;

	float useTimer_ = 0.0f;
};
