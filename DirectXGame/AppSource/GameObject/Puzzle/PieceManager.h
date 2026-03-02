#pragma once
#include <Game/Item/Item.h>
#include "Piece.h"

class PieceManager {
public:

	void Initialize();

	int AddPiece(const Item& item);
	std::vector<Piece> GetPieces();

private:

	std::map<int, Piece> pieces_;
	int nextID_ = 0;

};
