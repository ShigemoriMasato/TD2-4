#pragma once
#include <GameObject/Item/ItemManager.h>
#include <Shop/Piece.h>
#include <random>

class Shop {
public:

	void Initialize(ItemManager* itemManager);

	std::vector<std::unique_ptr<Piece>> RefreshShopPieces();

	struct Config {
		int stageNum = 0;
	} config;

private:

	ItemManager* itemManager_;
	std::mt19937 randomEngine_{ std::random_device{}() };
	std::vector<int> itemIndices_;
};
