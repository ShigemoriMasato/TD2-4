#pragma once
#include "Piece.h"
#include <memory>
#include <GameObject/Item/ItemManager.h>

class PieceManager {
public:

	struct BreakEffectInfo {
		Vector3 position;
		int weaponID;
		Piece::Direction direction;
	};

	void Initialize();

	void UpdateItemInfo(ItemManager* itemManager);
	std::vector<Piece*> Update(BackPack* backPack, float deltaTime);

	// ショップのピースを更新する。ショップのピースは別のクラスで抽選したものを引数で渡す
	void RefreshShopPieces(std::vector<std::unique_ptr<Piece>> shopPieces);

	//ShopからHoldに移動させる（マージ判定付き）
	void MoveShopToHold(Piece* piece, BackPack* backPack);

	//削除
	void RemovePiece(Piece* piece);
	// 削除 + パーティクル用に位置を記録
	void RemovePieceWithEffect(Piece* piece, BackPack* backPack);

	// 右クリック削除時のパーティクル位置を取得してクリアする
	std::vector<BreakEffectInfo> TakeDeletePositions() {
		std::vector<BreakEffectInfo> result = std::move(pendingDeletePositions_);
		pendingDeletePositions_.clear();
		return result;
	}

	// 場にあるピースを全て取得する。当たり判定用
	std::vector<Piece*> GetAllPieces();

	// ショップにあるピースの数を取得
	size_t GetShopPieceCount() const { return shopPieces_.size(); }

	// 消えたショップピースの位置とweaponIDを取得してクリアする
	std::vector<BreakEffectInfo> TakeBreakPositions() {
		std::vector<BreakEffectInfo> result = std::move(pendingBreakPositions_);
		pendingBreakPositions_.clear();
		return result;
	}

	// マージ対象を探す：同様のアイテムID・同レアリティでチップが1つ以上重なるピースを返す
	Piece* FindMergeTarget(Piece* piece);

	// ピースがショップエリアにあるかを判定
	bool IsShopPiece(Piece* piece) const;

	// 現在のショップピース全てのブレイクエフェクトを即座に記録し、描画からも除外する
	void EmitShopBreakEffects() {
		for (const auto& piece : shopPieces_) {
			pendingBreakPositions_.push_back({ piece->GetPosition(), piece->GetItem().weaponID, piece->GetDirection() });
		}
		// allPieces_ からショップピースを取り除いて即座に非表示にする
		allPieces_.erase(std::remove_if(allPieces_.begin(), allPieces_.end(),
			[this](Piece* p) {
				for (const auto& sp : shopPieces_) {
					if (sp.get() == p) return true;
				}
				return false;
			}), allPieces_.end());
	}

	// ホールドに直接ピースを追加する（初期武器付与などに使用）
	void AddHoldPiece(std::unique_ptr<Piece> piece) {
		allPieces_.push_back(piece.get());
		holdPieces_.push_back(std::move(piece));
	}

private:

	std::vector<BreakEffectInfo> pendingBreakPositions_;
	std::vector<BreakEffectInfo> pendingDeletePositions_;

	std::vector<std::unique_ptr<Piece>> shopPieces_;
	std::vector<std::unique_ptr<Piece>> holdPieces_;

	std::vector<Piece*> allPieces_;
};
