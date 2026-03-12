#pragma once
#include "Piece.h"
#include <memory>
#include <GameObject/Item/ItemManager.h>

class PieceManager {
public:

	void Initialize();

	void UpdateItemInfo(ItemManager* itemManager);

	// ショップのピースを更新する。ショップのピースは別のクラスで抽選したものを引数で渡す
	void RefreshShopPieces(std::vector<std::unique_ptr<Piece>> shopPieces);

	//ShopからHoldに移動させる
	void MoveShopToHold(Piece* piece);

	//削除
	void RemovePiece(Piece* piece);

	// 場にあるピースを全て取得する。当たり判定用
	std::vector<Piece*> GetAllPieces();
	// 最後保存する用
	void GetHoldPieces(std::vector<Piece*>& pieces);

private:

	std::vector<std::unique_ptr<Piece>> shopPieces_;
	std::vector<std::unique_ptr<Piece>> holdPieces_;

	std::vector<Piece*> allPieces_;

};
