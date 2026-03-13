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

	void DrawImGui();

private:

	ItemManager* itemManager_;
	std::mt19937 randomEngine_{ std::random_device{}() };
	std::vector<int> itemIndices_;

	Vector3 startPos_{ 0.0f, 0.0f, 8.0f };
	Vector3 interval_{ 4.0f, 0.0f, 0.0f };
};
