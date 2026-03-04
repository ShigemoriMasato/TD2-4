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
	for (int y = 0; y < GameConstants::kBackPackRowNum; ++y)
	{
		for (int x = 0; x < GameConstants::kBackPackColNum; ++x)
		{
			pieces_[y][x]->Initialize(GridState::UnlockedEmpty);
			pieces_[y][x]->SetPosition(GetPositionByIndex(Vector2int(x, y)));
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


bool BackPack::SetItem(Vector2int mapIndex, Item* item, const int itemlocalShapeIndex)
{
	return pieces_[mapIndex.y][mapIndex.x]->SetItem(item, itemlocalShapeIndex);
}

bool BackPack::IsEmpty(const Vector2int& index) const
{
	if (pieces_[index.y][index.x]->GetState() == GridState::UnlockedEmpty)
	{
		return true;
	}
	return false;
}
bool BackPack::IsEmpty(const Vector3& pos) const
{
	return IsEmpty(GetIndexByPosition(pos));
}


Vector2int BackPack::GetIndexByPosition(const Vector3& pos) const
{
	const float cellSize = GameConstants::kBackPackCellSize;
	const float spacing = GameConstants::kBackPackCellSpacing;

	float xIndex = pos.x / (cellSize + spacing);
	float yIndex = pos.z / (cellSize + spacing);

	return Vector2int{ int(xIndex), int(yIndex) };
}
Vector3 BackPack::GetPositionByIndex(const Vector2int& index) const
{
	const float cellSize = GameConstants::kBackPackCellSize;
	const float spacing = GameConstants::kBackPackCellSpacing;

	const float x = float(index.x) * (cellSize + spacing);
	const float z = float(index.y) * (cellSize + spacing);

	return Vector3(x, 0.0f, z);
}

