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

	bool HasHeldPiece() const { return heldPiece_ != nullptr; }

	bool GetIsEffect() const { return isEffect_; }
	Vector3 GetPutPos() const { return putPos_; }
	void SetVPMatrix(Matrix4x4 vpMatrix) { vpMatrix_ = vpMatrix; }

private:

	KeyManager* keyManager_ = nullptr;
	PieceManager* pieceManager_ = nullptr;
	Vector3 worldPos_ = { 0.0f, 0.0f, 0.0f };

	Piece* heldPiece_ = nullptr;
	Piece::Direction preHeldPieceDir_ = Piece::Direction::Up;
	Vector3 preHeldPiecePos_ = { 0.0f, 0.0f, 0.0f };

	bool isEffect_ = false;
	Vector3 putPos_;
	Matrix4x4 vpMatrix_;
};
