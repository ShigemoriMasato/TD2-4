#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Camera/Camera.h>
#include "PieceManager.h"
#include "Piece.h"

class ShopCursor {
public:

	void Initialize(KeyManager* keyManager, PieceManager* pieceManager);

	void Update(Camera* camera);
	void EditPiece(BackPack* backPack, float deltaTime);

	bool HasHeldPiece() const { return heldPiece_ != nullptr; }

	bool GetIsEffect() const { return isEffect_; }
	Vector3 GetPutPos() const { return putPos_; }
	int GetPutWeaponID() const { return putWeaponID_; }
	bool GetPutIsVertical() const { return putIsVertical_; }
	Piece::Direction GetPutDirection() const { return putDirection_; }
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
	int putWeaponID_ = -1;
	bool putIsVertical_ = false;
	Piece::Direction putDirection_ = Piece::Direction::Up;
	Matrix4x4 vpMatrix_;

	// BackPack内ピースの右クリック長押し削除用
	Piece* rightClickTarget_ = nullptr;
	float rightClickHoldTimer_ = 0.0f;
	static constexpr float kRightClickDeleteTime_ = 1.0f;
};
