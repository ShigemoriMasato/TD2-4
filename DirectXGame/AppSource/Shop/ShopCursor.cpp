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
				if (!piece->IsHoldOutside() && piece->GetItem().category == Category::Weapon) {
					auto originalPos = piece->GetPosition();
					auto originalDir = piece->GetDirection();

					bool moved = false;
					Piece::Direction foundDir = originalDir;
					Vector3 foundPos = originalPos;

					int startZ = static_cast<int>(backPack->GetOriginPos().z) - 1;
					for (int z = startZ; z >= startZ - 10 && !moved; z--) {
						for (int xOffset = 0; xOffset <= 10 && !moved; xOffset++) {
							for (int xSign = -1; xSign <= 1 && !moved; xSign += 2) {
								int x = xOffset * xSign;
								for (int dir = 0; dir < 4 && !moved; dir++) {
									piece->SetPosition(Vector3(static_cast<float>(x), 0.0f, static_cast<float>(z)));
									if (piece->CanHoldOutside(backPack, pieces)) {
										moved = true;
										foundPos = piece->GetPosition();
										foundDir = piece->GetDirection();
									} else {
										piece->RotateRight();
									}
								}
								if (xOffset == 0) break; // x=0は1度だけ
							}
						}
					}

					if (moved) {
						// 元の場所・向きに戻してRemoveでマス情報をクリア
						piece->SetPosition(originalPos);
						while(piece->GetDirection() != originalDir) {
							piece->RotateRight();
						}
						piece->Remove(backPack);

						// 見つけた場所・向きに再度移動してHoldOutside
						piece->SetPosition(foundPos);
						while(piece->GetDirection() != foundDir) {
							piece->RotateRight();
						}
						piece->HoldOutside();
					} else {
						// 置けなかった場合は元に戻してUseを呼ぶ
						piece->SetPosition(originalPos);
						while(piece->GetDirection() != originalDir) {
							piece->RotateRight();
						}
						piece->Use();
					}
				} else if (piece->IsHoldOutside()) {
					auto originalPos = piece->GetPosition();
					auto originalDir = piece->GetDirection();

					bool moved = false;
					Piece::Direction foundDir = originalDir;
					Vector3 foundPos = originalPos;

					int startX = static_cast<int>(backPack->GetOriginPos().x);
					int startZ = static_cast<int>(backPack->GetOriginPos().z);

					for (int z = startZ; z < startZ + 10 && !moved; z++) {
						for (int xOffset = 0; xOffset <= 10 && !moved; xOffset++) {
							for (int xSign = -1; xSign <= 1 && !moved; xSign += 2) {
								int x = startX + xOffset * xSign;
								if (x < startX || x >= startX + 10) {
									if (xOffset == 0) break;
									continue;
								}
								
								for (int dir = 0; dir < 4 && !moved; dir++) {
									piece->SetPosition(Vector3(static_cast<float>(x), 0.0f, static_cast<float>(z)));
									if (piece->CanPut(backPack)) {
										moved = true;
										foundPos = piece->GetPosition();
										foundDir = piece->GetDirection();
									} else {
										piece->RotateRight();
									}
								}
								if (xOffset == 0) break; // x=0は1度だけ
							}
						}
					}

					if (moved) {
						// 見つけた場所・向きに移動してバックパックに配置 (Hold状態から復帰)
						piece->SetPosition(foundPos);
						while(piece->GetDirection() != foundDir) {
							piece->RotateRight();
						}
						piece->Put(backPack);
					} else {
						// 置けなかった場合は元に戻すだけで何も起こらない
						piece->SetPosition(originalPos);
						while(piece->GetDirection() != originalDir) {
							piece->RotateRight();
						}
					}
				} else {
					piece->Use();
				}
			}

			break;
		}
	}
}
