#include "ShopCursor.h"
#include <Common/KeyConfig/WorldCursor.h>

void ShopCursor::Initialize(KeyManager* keyManager) {
	keyManager_ = keyManager;
}

void ShopCursor::Update(Camera* camera) {
	Vector2 cursor = keyManager_->GetCursorPos();
	worldPos_ = GetWorldCursor(camera, cursor);
}

void ShopCursor::EditPiece(const std::vector<std::unique_ptr<Piece>>& pieces, BackPack* backPack) {
	//持っているピースがあるなら
	if (heldPiece_) {
		if(!keyManager_->GetKeyStates()[Key::Hold]) {
			//本当にその場所に配置できるかの判断を行う
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
				heldPiece_ = piece.get();
				preHeldPiecePos_ = piece->GetPosition();
			}

			break;
		}
	}
}
