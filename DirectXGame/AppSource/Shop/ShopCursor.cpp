#include "ShopCursor.h"
#include <cmath>
#include <Common/KeyConfig/WorldCursor.h>
#include <Utils/AppUtils.h>
#include <GameObject/Weapon/WeaponData.h>

void ShopCursor::Initialize(KeyManager* keyManager, PieceManager* pieceManager) {
	keyManager_ = keyManager;
	pieceManager_ = pieceManager;
}

void ShopCursor::Update(Camera* camera) {
	Vector2 cursor = keyManager_->GetCursorPos();
	//x = 48.0f ~ 400.0f		width 352.0f
	//y = 48.0f ~ 672.0f		width 624.0f
	cursor.x = (cursor.x - 48.0f) / (352.0f / 1280.0f);
	cursor.y = (cursor.y - 48.0f) / (624.0f / 720.0f);

	worldPos_ = GetWorldCursor(camera, cursor);
	worldPos_.x -= 0.5f;
	worldPos_.y -= 0.5f;
}

void ShopCursor::EditPiece(BackPack* backPack, float deltaTime) {
	auto pieces = pieceManager_->GetAllPieces();
	auto keys = keyManager_->GetKeyStates();

	isEffect_ = false;

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

			// 以前の保留状態をリセット
			bool wasReserved = heldPiece_->IsReserved();
			heldPiece_->SetReserved(false);

			while (currentDir != tmpDir) {
				heldPiece_->RotateRight();
				currentDir = heldPiece_->GetDirection();
			}
			heldPiece_->SetPosition(worldPos_ - heldPiece_->GetCenterOffset());

			//おけるなら配置、無理なら元の場所に戻す
			if (heldPiece_->CanPut(backPack)) {
				//配置する
				heldPiece_->Put(backPack);

				// ゲージに吸われるエフェクトの発火
				isEffect_ = true;
				putPos_ = heldPiece_->GetPosition();

			} else if (Piece* mergeTarget = pieceManager_->FindMergeTarget(heldPiece_)) {
				// 同種・同レアリティのピースに重ねた → マージ
				mergeTarget->Remove(backPack);
				pieceManager_->RemovePiece(mergeTarget);
				heldPiece_->SetRarity(static_cast<WeaponRarity>(static_cast<int>(heldPiece_->GetRarity()) + 1));
				if (heldPiece_->CanPut(backPack)) {
					heldPiece_->Put(backPack);

					isEffect_ = true;
					putPos_ = heldPiece_->GetPosition();
				} else {
					// マージ後も置けない場合は元の場所に戻す
					heldPiece_->SetRarity(static_cast<WeaponRarity>(static_cast<int>(heldPiece_->GetRarity()) - 1));
					auto currentDir = heldPiece_->GetDirection();
					while (currentDir != preHeldPieceDir_) {
						heldPiece_->RotateRight();
						currentDir = heldPiece_->GetDirection();
					}
					heldPiece_->SetPosition(preHeldPiecePos_);
					heldPiece_->SetReserved(wasReserved);
					if (heldPiece_->CanPut(backPack)) {
						heldPiece_->Put(backPack);
					}
				}
			} else {
				// 元の場所に戻す
				auto currentDir = heldPiece_->GetDirection();
				while (currentDir != preHeldPieceDir_) {
					heldPiece_->RotateRight();
					currentDir = heldPiece_->GetDirection();
				}
				heldPiece_->SetPosition(preHeldPiecePos_);
				heldPiece_->SetReserved(wasReserved); // 元の保留状態に戻す
				// Putメソッドを使って元の場所に配置し直す
				if (heldPiece_->CanPut(backPack)) {
					heldPiece_->Put(backPack);
				}
			}

			// ピースを離すときに持たれている状態を解除
			heldPiece_->SetHeld(false);
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
				// ピースを持つときに持たれている状態を設定
				heldPiece_->SetHeld(true);
			}

			if (keys[Key::Erase]) {
				piece->Remove(backPack);
				pieceManager_->RemovePiece(piece);
			}

			if (keys[Key::Use]) {
				piece->Use();
			}

			// 右クリック長押し（AutoPlace）によるBackPack削除タイマー処理
			if (!pieceManager_->IsShopPiece(piece)) {
				if (keys[Key::AutoPlace]) {
					if (rightClickTarget_ != piece) {
						rightClickTarget_ = piece;
						rightClickHoldTimer_ = 0.0f;
					}
					rightClickHoldTimer_ += deltaTime;
					// sin/cos波でX・Z両軸シェイク（時間とともに振幅増大）
					float progress = rightClickHoldTimer_ / kRightClickDeleteTime_;
					float amplitude = 0.05f + 0.15f * progress;
					float shakeX = std::sin(rightClickHoldTimer_ * 30.0f) * amplitude;
					float shakeZ = std::cos(rightClickHoldTimer_ * 30.0f) * amplitude;
					piece->SetShakeOffset(shakeX, shakeZ);
					if (rightClickHoldTimer_ >= kRightClickDeleteTime_) {
						piece->ResetShakeOffset();
						pieceManager_->RemovePieceWithEffect(piece, backPack);
						rightClickTarget_ = nullptr;
						rightClickHoldTimer_ = 0.0f;
						break;
					}
				} else {
					if (rightClickTarget_ == piece) {
						piece->ResetShakeOffset();
						rightClickTarget_ = nullptr;
						rightClickHoldTimer_ = 0.0f;
					}
				}
			}

			// 右クリック（AutoPlace）の処理
			if (keys[Key::AutoPlace]) {
				// ショップエリアのピースかを確認
				if (pieceManager_->IsShopPiece(piece)) {
					// 元の位置と回転を保存
					Vector3 originalPos = piece->GetPosition();
					Piece::Direction originalDir = piece->GetDirection();
					
					// 自動配置を試みる
					if (piece->AutoPlace(backPack)) {
						// 配置成功：何もしない（既にPutが呼ばれている）

						// ゲージに吸われるエフェクトの発火
						isEffect_ = true;
						putPos_ = piece->GetPosition();
						
					} else {
						// 配置失敗：元の位置と回転に戻す
						// 回転を元に戻す
						while (piece->GetDirection() != originalDir) {
							piece->RotateRight();
						}
						piece->SetPosition(originalPos);
					}
				} else {
					// BackPack内のアイテムの場合
					if (piece->IsReserved()) {
						// 保留エリアにある場合、通常エリアに移動
						piece->MoveToNormal(backPack);

						// ゲージに吸われるエフェクトの発火
						isEffect_ = true;
						putPos_ = piece->GetPosition();
					} else {
						// 通常エリアにある場合、保留エリアに移動
						piece->MoveToReserve(backPack);
					}
				}
			}

			break;
		}
	}
}
