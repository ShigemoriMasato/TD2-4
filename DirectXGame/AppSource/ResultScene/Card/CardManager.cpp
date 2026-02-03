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
	float scale = 1.5f - 0.1f * float(cards_.size() - 1);
	scale = std::max(scale, 0.9f);
	const float cardSize = 200.0f;
	float xLeft = 640.0f - (float(cards_.size() - 1) * cardSize * scale) / 2.0f;
	float yUp = ((cards_.size() - 1) / 6) * -cardSize * scale / 2.0f + 360.0f;
	bool isFinished = true;
	for(int i = 0; i < cards_.size(); i++) {
		Vector3 position;
		position.x = xLeft + (i % 6) * cardSize * scale;
		position.y = yUp + (i / 6) * -cardSize * scale;
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
	for (auto& card : cards_) {
		card->Draw(window, vpMat);
	}
}

void CardManager::AddCard(int score, int norma) {
	auto& card = cards_.emplace_back(std::make_unique<Card>());
	card->Initialize("YDWbananaslipplus.otf", drawData_, fontLoader_, textureManager_, int(cards_.size()), score < norma);
	card->SetNumber(score, norma);
}
