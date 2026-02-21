#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Game/Item/Item.h>

class Piece {
public:

	Piece() : id_(nextID_++) {}

	void SetItem(const Item& item);

	void SetPosition(const Vector3& position) { position_ = position; }

	Vector3 GetPosition() const { return position_; }
	int GetModelID() const { return item_.modelID; }

private:

	Item item_;
	Vector3 position_;

	static inline int nextID_ = 0;

};
