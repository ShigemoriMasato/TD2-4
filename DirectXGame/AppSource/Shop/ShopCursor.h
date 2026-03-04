#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Camera/Camera.h>
#include "Piece.h"

class ShopCursor {
public:

	void Initialize(KeyManager* keyManager);

	void Update(Camera* camera);
	void EditPiece(const std::vector<std::unique_ptr<Piece>>& pieces, BackPack* backPack);

private:

	KeyManager* keyManager_ = nullptr;
	Vector3 worldPos_ = { 0.0f, 0.0f, 0.0f };

	Piece* heldPiece_ = nullptr;
	Vector3 preHeldPiecePos_ = { 0.0f, 0.0f, 0.0f };

};
