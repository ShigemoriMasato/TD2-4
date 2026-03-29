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

	//ShopからHoldに移動させる
	void MoveShopToHold(Piece* piece);

	//削除
	void RemovePiece(Piece* piece);

	// 場にあるピースを全て取得する。当たり判定用
	std::vector<Piece*> GetAllPieces();

	// ピースがショップエリアにあるかを判定
	bool IsShopPiece(Piece* piece) const;

private:

	std::vector<std::unique_ptr<Piece>> shopPieces_;
	std::vector<std::unique_ptr<Piece>> holdPieces_;

	std::vector<Piece*> allPieces_;

};
