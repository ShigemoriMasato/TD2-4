#include "ShopCursor.h"
#include <Common/KeyConfig/WorldCursor.h>

void ShopCursor::Initialize(KeyManager* keyManager, PieceManager* pieceManager) {
	keyManager_ = keyManager;
	pieceManager_ = pieceManager;
}

void ShopCursor::Update(Camera* camera) {
	Vector2 cursor = keyManager_->GetCursorPos();
	cursor.x *= 2.0f;
	worldPos_ = GetWorldCursor(camera, cursor);
}

void ShopCursor::EditPiece(BackPack* backPack) {
	auto pieces = pieceManager_->GetAllPieces();
	auto keys = keyManager_->GetKeyStates();

	//持っているピースがあるなら
	if (heldPiece_) {
		if(keys[Key::Rotate]) {
			heldPiece_->RotateRight();
		}

		if (!keys[Key::Hold]) {
			//本当にその場所に配置できるかの判断を行う
			//配置前の状態を再現し、バックパックから削除する
			//元の場所の記録を削除してから、置けるかを判断する
			auto tmpDir = heldPiece_->GetDirection();
			auto currentDir = heldPiece_->GetDirection();
			while (currentDir != preHeldPieceDir_) {
				heldPiece_->RotateRight();
				currentDir = heldPiece_->GetDirection();
			}
			heldPiece_->SetPosition(preHeldPiecePos_);
			heldPiece_->Remove(backPack);//削除

			while (currentDir != tmpDir) {
				heldPiece_->RotateRight();
				currentDir = heldPiece_->GetDirection();
			}
			heldPiece_->SetPosition(worldPos_ - heldPiece_->GetCenterOffset());

			//おけるなら配置、無理なら元の場所に戻す
			if (heldPiece_->CanPut(backPack)) {
				//配置する
				heldPiece_->Put(backPack);

			} else if (heldPiece_->CanHoldOutside(backPack, pieces)) {
				//バックパックの外側に保持する
				heldPiece_->HoldOutside();

			} else {
				auto currentDir = heldPiece_->GetDirection();
				while (currentDir != preHeldPieceDir_) {
					heldPiece_->RotateRight();
					currentDir = heldPiece_->GetDirection();
				}
				heldPiece_->SetPosition(preHeldPiecePos_);
			}

			heldPiece_ = nullptr;

		} else {

			heldPiece_->SetPosition(worldPos_ - heldPiece_->GetCenterOffset());

		}

		return;
	}

	//持っているピースがないなら、ホバーしているピースを探す
	for (auto& piece : pieces) {
		if (piece->IsHovered(worldPos_, backPack)) {

			if (keys[Key::Hold]) {
				heldPiece_ = piece;
				preHeldPieceDir_ = heldPiece_->GetDirection();
				preHeldPiecePos_ = piece->GetPosition();
			}

			if (keys[Key::Erase]) {
				piece->Remove(backPack);
				pieceManager_->RemovePiece(piece);
			}

			if (keys[Key::Use]) {
				piece->Use();
			}

			break;
		}
	}
}
