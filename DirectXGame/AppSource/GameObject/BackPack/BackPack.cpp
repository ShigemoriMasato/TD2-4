#include "BackPack.h"
#include <Utility/Matrix.h>
#include "GameObject/Item/ItemManager.h"
#include "gameObject/BackPack/BackPackPiece/BackPackPiece.h"
#include "gameObject/BackPack/Shop/Shop.h"


BackPack::BackPack()
{
	drawBackPack_ = std::make_unique<DrawBackPack>();

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

void BackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData, SHEngine::Input* input)
{
	drawBackPack_->Initialize(modelManager, drawDataManager);

	// 初期マップ
	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			pieces_[r][c]->Initialize(GridState::UnlockedEmpty);
			pieces_[r][c]->SetPosition(Vector3(float(c) * 2.0f, 0.0f, float(r) * 2.0f));
		}
	}
}

void BackPack::Update(const Matrix4x4& viewProj)
{
	drawBackPack_->Update(viewProj, pieces_);
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	drawBackPack_->Draw(cmdObject);
}

void BackPack::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("BackPack");

	ImGui::End();
#endif // USE_IMGUI
}




