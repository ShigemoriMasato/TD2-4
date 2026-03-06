#pragma once
#include "Piece.h"
#include <memory>

class PieceManager {
public:

	void Initialize(std::vector<std::unique_ptr<Piece>>&& playerPieces);

	// ショップのピースを更新する。ショップのピースは別のクラスで抽選したものを引数で渡す
	void RefreshShopPieces(std::vector<std::unique_ptr<Piece>> shopPieces);

	//ShopからHoldに移動させる
	void MoveShopToHold(Piece* piece);

	// 場にあるピースを全て取得する。当たり判定用
	std::vector<Piece*> GetAllPieces();
	// 最後保存する用
	std::vector<std::unique_ptr<Piece>>&& GetHoldPieces();

private:

	std::vector<std::unique_ptr<Piece>> shopPieces_;
	std::vector<std::unique_ptr<Piece>> holdPieces_;

	std::vector<Piece*> allPieces_;

};
