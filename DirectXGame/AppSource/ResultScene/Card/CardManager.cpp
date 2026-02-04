#include "CardManager.h"

namespace {
	float timer = 0.0f;
}

void CardManager::Initialize(const DrawData& drawData, FontLoader* fontLoader, TextureManager* textureManager, const std::vector<std::pair<int, int>>& scores) {
	drawData_ = drawData;
	fontLoader_ = fontLoader;
	textureManager_ = textureManager;
	cards_.clear();
	scores_ = scores;
	timer = 0.f;
	worldMat_ = Matrix::MakeTranslationMatrix({ -720.0f, 0.0f, 0.0f });
}

bool CardManager::Update(float deltaTime) {
	const float cardSize = 260.0f;
	const int row = 5;

	bool isFinished = true;
	for (int i = 0; i < cards_.size(); i++) {
		Vector3 position;

		int retsu = i / row;
		int gyou = i % row;
		int gyouNum = std::min(row, ((int)cards_.size() - retsu * row));
		int retsuNum = int(cards_.size() - 1) / row;

		float left = -cardSize * float(gyouNum - 1) * 0.5f + 640.f;
		float high = cardSize * float(retsuNum) * 0.5f + 360.f;
		float scale = 1.2f - 0.1f * gyouNum;

		position.x = left + cardSize * float(i % row);
		position.y = high - cardSize * float(i / row);
		position.z = 0.0f;
		cards_[i]->SetTransData(position, scale);
		isFinished = cards_[i]->Update(deltaTime, worldMat_);
	}

	if (!scores_.empty()) {
		timer += deltaTime;
	}

	if (timer >= 0.4f) {
		timer = 0.0f;
		auto score = scores_.begin();
		AddCard(score->first, score->second);
		scores_.erase(score);
	}

	return scores_.empty() && isFinished;
}

void CardManager::Draw(Window* window, const Matrix4x4 vpMat) {
	if (cards_.empty()) return;
	for(const auto& card : cards_)
		card->Draw(window, vpMat);
	cards_[0]->DrawImGui();
}

void CardManager::AddCard(int score, int norma) {
	auto& card = cards_.emplace_back(std::make_unique<Card>());
	card->Initialize("YDWbananaslipplus.otf", drawData_, fontLoader_, textureManager_, int(cards_.size()), score < norma);
	card->SetNumber(score, norma);
}
