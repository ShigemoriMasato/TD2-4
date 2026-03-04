#pragma once
#include <GameObject/Item/Item.h>
#include "BackPack.h"

class Piece {
public:

	Piece() = default;

	void Initialize(Item& item);

	void SetPosition(const Vector3& pos) { position_ = pos; }

	Vector3 GetPosition() const { return position_; }
	bool CanPut(BackPack* backPack) const;
	bool Put(BackPack* backPack);
	bool IsHovered(const Vector3& cursorPos, BackPack* backPack) const;

private:

	Item itemData_;
	std::vector<std::pair<int, int>> chips_;

	Vector3 position_{};

	Vector2 maxLocalPos_{};

};
