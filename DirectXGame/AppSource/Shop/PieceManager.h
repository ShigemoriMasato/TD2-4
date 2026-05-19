#pragma once
#include "Piece.h"
#include <memory>
#include <GameObject/Item/ItemManager.h>

class PieceManager {
public:

	void Initialize();

	void UpdateItemInfo(ItemManager* itemManager);
	std::vector<Piece*> Update(BackPack* backPack, float deltaTime);

	// ショップのピースを更新する。ショップのピースは別のクラスで抽選したものを引数で渡す
	void RefreshShopPieces(std::vector<std::unique_ptr<Piece>> shopPieces);

	//ShopからHoldに移動させる（マージ判定付き）
	void MoveShopToHold(Piece* piece, BackPack* backPack);

	//削除
	void RemovePiece(Piece* piece);

	// 場にあるピースを全て取得する。当たり判定用
	std::vector<Piece*> GetAllPieces();

	// ショップにあるピースの数を取得
	size_t GetShopPieceCount() const { return shopPieces_.size(); }

	// 消えたショップピースの位置を取得してクリアする
	std::vector<Vector3> TakeBreakPositions() {
		std::vector<Vector3> result = std::move(pendingBreakPositions_);
		pendingBreakPositions_.clear();
		return result;
	}

	// マージ対象を探す：同様のアイテムID・同レアリティでチップが1つ以上重なるピースを返す
	Piece* FindMergeTarget(Piece* piece);

	// ピースがショップエリアにあるかを判定
	bool IsShopPiece(Piece* piece) const;

private:

	std::vector<Vector3> pendingBreakPositions_;

	std::vector<std::unique_ptr<Piece>> shopPieces_;
	std::vector<std::unique_ptr<Piece>> holdPieces_;

	std::vector<Piece*> allPieces_;
};
