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

	ImGui::Begin("Cursor");
	ImGui::Text("Screen Pos: (%.2f, %.2f)", cursor.x, cursor.y);
	ImGui::Text("World Pos: (%.2f, %.2f, %.2f)", worldPos_.x, worldPos_.y, worldPos_.z);
	ImGui::End();
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
			//おけるなら配置、無理なら元の場所に戻す
			if (heldPiece_->CanPut(backPack)) {
				//配置前の状態を再現し、バックパックから削除する
				auto tmpDir = heldPiece_->GetDirection();
				auto currentDir = heldPiece_->GetDirection();
				while(currentDir != preHeldPieceDir_) {
					heldPiece_->RotateRight();
					currentDir = heldPiece_->GetDirection();
				}
				heldPiece_->SetPosition(preHeldPiecePos_);
				heldPiece_->Remove(backPack);

				//状態を戻し、配置する
				preHeldPieceDir_ = tmpDir;
				while (currentDir != preHeldPieceDir_) {
					heldPiece_->RotateRight();
					currentDir = heldPiece_->GetDirection();
				}
				heldPiece_->SetPosition(worldPos_);
				heldPiece_->Put(backPack);

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

			heldPiece_->SetPosition(worldPos_);

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

			if(keys[Key::Rotate]) {
				piece->RotateRight();
			}

			if (keys[Key::Use]) {
				piece->Use();
			}

			break;
		}
	}
}
