#include "BackPack.h"
#include <Utility/Matrix.h>
#include "GameObject/Item/ItemManager.h"
#include "gameObject/BackPack/BackPackPiece/BackPackPiece.h"
#include "gameObject/BackPack/Shop/Shop.h"


BackPack::BackPack()
{
	drawBackPack_ = std::make_unique<DrawBackPack>();

	shop_ = std::make_unique<Shop>();

	for (int i = 0; i < GameConstants::kBackPackRowNum; ++i)
	{
		std::vector<std::unique_ptr<BackPackPiece>> row;
		for (int j = 0; j < GameConstants::kBackPackColNum; ++j)
		{
			row.push_back(std::make_unique<BackPackPiece>());
		}
		pieces_.push_back(std::move(row));
	}
}

BackPack::~BackPack() {};

void BackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData)
{
	drawBackPack_->Initialize(modelManager, drawDataManager);

	shop_->Initialize(modelManager, drawDataManager, itemManager, commonData);

	// 初期マップ
	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			pieces_[r][c]->Initialize(GridState::UnlockedEmpty);
			pieces_[r][c]->SetPosition(Vector3(float(c), 0.0f, float(r)));
		}
	}
}

void BackPack::Update(const Matrix4x4& viewProj)
{
	drawBackPack_->Update(viewProj, pieces_);

	shop_->Update(viewProj);
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	drawBackPack_->Draw(cmdObject);

	shop_->Draw(cmdObject);
}

void BackPack::DrawImGui()
{
	ImGui::Begin("BackPack");

	shop_->DrawImGui();

	ImGui::End();
}




