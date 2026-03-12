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

	//持っているピースがあるなら
	if (heldPiece_) {
		if(keyManager_->GetKeyStates()[Key::Rotate]) {
			heldPiece_->RotateRight();
		}

		if (!keyManager_->GetKeyStates()[Key::Hold]) {
			//本                                     当にその場所に配置できるかの判断を行う
			if (heldPiece_->CanPut(backPack)) {
				heldPiece_->Put(backPack);
			}
			//おけるなら配置、無理なら元の場所に戻す
			else {
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

			if (keyManager_->GetKeyStates()[Key::Hold]) {
				heldPiece_ = piece;
				preHeldPiecePos_ = piece->GetPosition();
			}

			if (keyManager_->GetKeyStates()[Key::Erase]) {
				pieceManager_->RemovePiece(piece);
			}

			break;
		}
	}
}
