#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Camera/Camera.h>
#include "PieceManager.h"
#include "Piece.h"

class ShopCursor {
public:

	void Initialize(KeyManager* keyManager, PieceManager* pieceManager);

	void Update(Camera* camera);
	void EditPiece(BackPack* backPack);

private:

	KeyManager* keyManager_ = nullptr;
	PieceManager* pieceManager_ = nullptr;
	Vector3 worldPos_ = { 0.0f, 0.0f, 0.0f };

	Piece* heldPiece_ = nullptr;
	Vector3 preHeldPiecePos_ = { 0.0f, 0.0f, 0.0f };
	Piece::Direction preHeldPieceDir_ = Piece::Direction::Up;

};
