#include "BackPack.h"
#include <Utility/Matrix.h>



BackPackGrid::BackPackGrid() {}

BackPackGrid::~BackPackGrid() {}

void BackPackGrid::Initialize(GridState state)
{
	state_ = state;
	ChangeState(state_);
}

void BackPackGrid::ChangeState(GridState newState)
{
	switch (state_)
	{
	// ロック中・解放不可
	case GridState::LockedUnavailable:
		transform_.scale = { 0.0f, 0.0f, 0.0f };
		color_ = { 0.25f, 0.25f, 0.25f, 1.0f };
		break;
	// ロック中・解放可能
	case GridState::LockedAvailable:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = -0.1f;
		color_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		break;
	// 解放済み・空
	case GridState::UnlockedEmpty:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = 0.0f;
		color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	// 解放済み・アイテムあり
	case GridState::UnlockedOccupied:
		transform_.scale = { 0.9f, 0.1f, 0.9f };
		transform_.position.y = 0.0f;
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
	const uint32_t maxInstance = uint32_t(GameConstants::kBackPackRowNum * GameConstants::kBackPackColNum);
	const int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");
	const auto modelData = modelManager->GetNodeModelData(modelHandle);
	const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	// 共通TextureIndex（Cubeのマテリアル）
	textureIndex_ = 0;
	if (!modelData.materialIndex.empty())
	{
		const auto& mat = modelData.materials[modelData.materialIndex.front()];
		textureIndex_ = mat.textureIndex;
	}

	// 共通ライト
	light_ = {};
	light_.color = { 1,1,1,1 };
	light_.direction = { 0,1,0 };
	light_.intensity = 1.0f;

	auto initGridRender = [&](std::unique_ptr<SHEngine::RenderObject>& ro, InstanceBinding& bind)
		{
			ro->Initialize();
			ro->SetDrawData(drawData);
			ro->SetUseTexture(true);
			ro->psoConfig_.vs = "Game/Piece.VS.hlsl";
			ro->psoConfig_.ps = "Game/Piece.PS.hlsl";

			bind.matricesSrvIndex = ro->CreateSRV(sizeof(Matrix4x4), maxInstance, ShaderType::VERTEX_SHADER, "Matrices");
			bind.vpCbvIndex = ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ViewProj");
			bind.texCbvIndex = ro->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
			bind.colorCbvIndex = ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
			bind.lightCbvIndex = ro->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

			ro->instanceNum_ = 0;
		};

	initGridRender(lockedUnavailableGrid_, bindLockedUnavailable_);
	initGridRender(lockedAvailableGrid_, bindLockedAvailable_);
	initGridRender(unlockedEmptyGrid_, bindUnlockedEmpty_);

	// 初期状態
	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			grids_[r][c]->Initialize(GridState::UnlockedEmpty);
			grids_[r][c]->transform_.position = { float(c), 0.0f, float(r) };
		}
	}

	// worlds の予約
	lockedUnavailableWorlds_.reserve(maxInstance);
	lockedAvailableWorlds_.reserve(maxInstance);
	unlockedEmptyWorlds_.reserve(maxInstance);
}

void BackPack::UpdateRenderObject(
	SHEngine::RenderObject& ro,
	const InstanceBinding& bind,
	const Matrix4x4& viewProj,
	const std::vector<Matrix4x4>& worlds,
	const Vector4& color)
{
	// 0個なら描かない
	ro.instanceNum_ = static_cast<uint32_t>(worlds.size());
	if (ro.instanceNum_ == 0) return;

	// VS
	ro.CopyBufferData(bind.vpCbvIndex, &viewProj, sizeof(Matrix4x4));
	ro.CopyBufferData(bind.matricesSrvIndex, worlds.data(), sizeof(Matrix4x4) * worlds.size());

	// PS(3重バッファ対策で毎フレーム送る(2億3敗))
	ro.CopyBufferData(bind.texCbvIndex, &textureIndex_, sizeof(int));
	ro.CopyBufferData(bind.colorCbvIndex, &color, sizeof(Vector4));
	ro.CopyBufferData(bind.lightCbvIndex, &light_, sizeof(DirectionalLight));
}


void BackPack::Update(const Matrix4x4& viewProj)
{
	lockedUnavailableWorlds_.clear();
	lockedAvailableWorlds_.clear();
	unlockedEmptyWorlds_.clear();

	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			const auto& g = *grids_[r][c];

			Matrix4x4 world = Matrix::MakeAffineMatrix(
				g.transform_.scale,
				g.transform_.rotate,
				g.transform_.position
			);

			switch (g.GetState())
			{
			case GridState::LockedUnavailable:
				lockedUnavailableWorlds_.push_back(world);
				break;
			case GridState::LockedAvailable:
				lockedAvailableWorlds_.push_back(world);
				break;
			case GridState::UnlockedEmpty:
				unlockedEmptyWorlds_.push_back(world);
				break;
			case GridState::UnlockedOccupied:
				unlockedEmptyWorlds_.push_back(world);
			default:
				break;
			}
		}
	}

	// それぞれGPUへ転送
	UpdateRenderObject(*lockedUnavailableGrid_, bindLockedUnavailable_, viewProj, lockedUnavailableWorlds_, lockedUnavailableColor_);
	UpdateRenderObject(*lockedAvailableGrid_, bindLockedAvailable_, viewProj, lockedAvailableWorlds_, lockedAvailableColor_);
	UpdateRenderObject(*unlockedEmptyGrid_, bindUnlockedEmpty_, viewProj, unlockedEmptyWorlds_, unlockedEmptyColor_);
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	if (lockedUnavailableGrid_)
	{
		lockedUnavailableGrid_->Draw(cmdObject);
	}
	if (lockedAvailableGrid_)
	{
		lockedAvailableGrid_->Draw(cmdObject);
	}
	if (unlockedEmptyGrid_)
	{
		unlockedEmptyGrid_->Draw(cmdObject);
	}
}

void BackPack::DrawImGui()
{
	ImGui::Begin("BackPack");


	ImGui::End();


}