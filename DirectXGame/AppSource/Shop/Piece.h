#pragma once
#include <GameObject/Item/Item.h>
#include "BackPack.h"

class PieceManager;

class Piece {
public:

	Piece() = default;
	Piece(const Item& item) { Initialize(item); }

	static void SetPieceManager(PieceManager* manager) { pieceManager_ = manager; }

	void Initialize(const Item& item);

	void SetPosition(const Vector3& pos);

	Vector3 GetPosition() const { return position_; }
	bool CanPut(BackPack* backPack) ;
	bool Put(BackPack* backPack);
	bool IsHovered(const Vector3& cursorPos, BackPack* backPack) ;
	std::vector<DrawInfo> GetDrawInfos() const;
	Item GetItem() const { return itemData_; }

private:

	static inline PieceManager* pieceManager_ = nullptr;

	Item itemData_;
	std::vector<std::pair<int, int>> chips_;

	Vector3 position_{};

	Vector2 maxLocalPos_{};

	bool isHovered_ = false;
	bool isPlaced_ = false;

};
