#pragma once
#include "Card.h"

class CardManager {
public:

	void Initialize(const DrawData& drawData, FontLoader* fontLoader, TextureManager* textureManager, const std::vector<std::pair<int, int>>& scores);
	bool Update(float deltaTime);
	void Draw(Window* window, const Matrix4x4 vpMat);

	void Move();

private:

	void AddCard(int score, int norma);

	//カードを生成するときの素材
	DrawData drawData_;
	FontLoader* fontLoader_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	Matrix4x4 worldMat_{};

	std::vector<std::unique_ptr<Card>> cards_;
	std::vector<std::pair<int, int>> scores_;

	bool isActive_ = true;
};
