#include "BackPack.h"
#include <Utility/Matrix.h>



BackPackGrid::BackPackGrid() {}

BackPackGrid::~BackPackGrid() {}

void BackPackGrid::Initialize(GridState state)
{
	state_ = state;
	switch (state_)
	{
	case GridState::LockedUnavailable:
		color_ = { 0.25f, 0.25f, 0.25f, 1.0f };
		break;
	case GridState::LockedAvailable:
		color_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		break;
	case GridState::UnlockedEmpty:
		color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	case GridState::UnlockedOccupied:
		color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	default:
		break;
	}
}

void BackPackGrid::Update()
{
}








BackPack::BackPack()
{
	for (int i = 0; i < GameConstants::kBackPackRowNum; ++i)
	{
		std::vector<std::unique_ptr<BackPackGrid>> row;
		for (int j = 0; j < GameConstants::kBackPackColNum; ++j)
		{
			row.push_back(std::make_unique<BackPackGrid>());
		}
		grids_.push_back(std::move(row));
	}

	lockedUnavailableGrid_ = std::make_unique<SHEngine::RenderObject>("BackPackLockedUnavailable");
	lockedAvailableGrid_ = std::make_unique<SHEngine::RenderObject>("BackPackLockedAvailable");
	unlockedEmptyGrid_ = std::make_unique<SHEngine::RenderObject>("BackPackUnlockedEmpty");
}

BackPack::~BackPack()
{}

void BackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager)
{
	uint32_t srvSize = GameConstants::kBackPackRowNum * GameConstants::kBackPackColNum;

	const int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");
	const auto modelData = modelManager->GetNodeModelData(modelHandle);
	const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	lockedUnavailableGrid_->Initialize();
	lockedUnavailableGrid_->SetDrawData(drawData);
	lockedUnavailableGrid_->SetUseTexture(true);
	lockedUnavailableGrid_->psoConfig_.vs = "Game/Piece.VS.hlsl";
	lockedUnavailableGrid_->psoConfig_.ps = "Game/Piece.PS.hlsl";
	lockedMatricesSrvIndex_ = lockedUnavailableGrid_->CreateSRV(sizeof(Matrix4x4), srvSize, ShaderType::VERTEX_SHADER, "Matrices");
	lockedVpCbvIndex_ = lockedUnavailableGrid_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ViewProj");
	lockedTexIndexCbvIndex_ = lockedUnavailableGrid_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	lockedColorCbvIndex_ = lockedUnavailableGrid_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	lockedLightCbvIndex_ = lockedUnavailableGrid_->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");


	std::vector<Matrix4x4> worlds;
	worlds.reserve(srvSize);

	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			grids_[r][c]->Initialize(GridState::LockedUnavailable);

			auto& t = grids_[r][c]->transform_;
			t.position = { float(c), 0.0f, float(r) };

			worlds.push_back(Matrix::MakeAffineMatrix(t.scale, t.rotate, t.position));
		}
	}

	lockedUnavailableGrid_->CopyBufferData(lockedMatricesSrvIndex_, worlds.data(), sizeof(Matrix4x4) * worlds.size());
	lockedUnavailableGrid_->instanceNum_ = static_cast<uint32_t>(worlds.size());

	// 初期CBV（必須）
	Matrix4x4 vp = Matrix4x4::Identity();
	lockedUnavailableGrid_->CopyBufferData(lockedVpCbvIndex_, &vp, sizeof(Matrix4x4));

	int textureIndex = 0;
	if (!modelData.materialIndex.empty())
	{
		const auto& mat = modelData.materials[modelData.materialIndex.front()];
		textureIndex = mat.textureIndex;
	}
	lockedUnavailableGrid_->CopyBufferData(lockedTexIndexCbvIndex_, &textureIndex, sizeof(int));

	const Vector4 color = { 0.25f, 0.25f, 0.25f, 1.0f };
	lockedUnavailableGrid_->CopyBufferData(lockedColorCbvIndex_, &color, sizeof(Vector4));

	DirectionalLight light{};
	light.color = { 1,1,1,1 };
	light.direction = { 0,-1,0 };
	light.intensity = 1.0f;
	lockedUnavailableGrid_->CopyBufferData(lockedLightCbvIndex_, &light, sizeof(DirectionalLight));

	lockedWorlds_ = worlds;
	lockedTextureIndex_ = textureIndex;
	lockedColor_ = color;
	lockedLight_ = light;
}

void BackPack::Update(const Matrix4x4& viewProj)
{
	// VS
	lockedUnavailableGrid_->CopyBufferData(lockedVpCbvIndex_, &viewProj, sizeof(Matrix4x4));
	lockedUnavailableGrid_->CopyBufferData(
		lockedMatricesSrvIndex_,
		lockedWorlds_.data(),
		sizeof(Matrix4x4)* lockedWorlds_.size()
	);

	// PS（これが抜けると index_==1,2 の面が未初期化になりやすい）
	lockedUnavailableGrid_->CopyBufferData(lockedTexIndexCbvIndex_, &lockedTextureIndex_, sizeof(int));
	lockedUnavailableGrid_->CopyBufferData(lockedColorCbvIndex_, &lockedColor_, sizeof(Vector4));
	lockedUnavailableGrid_->CopyBufferData(lockedLightCbvIndex_, &lockedLight_, sizeof(DirectionalLight));

	lockedUnavailableGrid_->instanceNum_ = static_cast<uint32_t>(lockedWorlds_.size());
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	if (lockedUnavailableGrid_)
	{
		lockedUnavailableGrid_->Draw(cmdObject);
	}
	//if (lockedAvailableGrid_)
	//{
	//	lockedAvailableGrid_->Draw(cmdObject);
	//}
	//if (unlockedEmptyGrid_)
	//{
	//	unlockedEmptyGrid_->Draw(cmdObject);
	//}
}