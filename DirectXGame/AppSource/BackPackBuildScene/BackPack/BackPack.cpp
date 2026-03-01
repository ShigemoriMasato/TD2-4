#include "BackPack.h"
#include <Utility/Matrix.h>
#include "Game/Item/ItemManager.h"



BackPackGrid::BackPackGrid() {}

BackPackGrid::~BackPackGrid() {}

void BackPackGrid::Initialize(GridState state)
{
	state_ = state;
	ChangeState(state_);
}

void BackPackGrid::ChangeState(GridState newState)
{
	switch (newState)
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

	state_ = newState;
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

	drawBackPack_ = std::make_unique<DrawBackPack>();
}

BackPack::~BackPack()
{}

void BackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager)
{
	drawBackPack_->Initialize(modelManager, drawDataManager);

	// 追加：ショップ5個をここで確定
	InitializeShopItems_(modelManager, drawDataManager, itemManager);

	// 初期マップ
	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			grids_[r][c]->Initialize(GridState::UnlockedEmpty);
			grids_[r][c]->transform_.position = { float(c), 0.0f, float(r) };
		}
	}
}

void BackPack::Update(const Matrix4x4& viewProj)
{
	drawBackPack_->Update(viewProj, grids_);

	// shop: 毎フレーム 3重バッファ面へ CBV 転送
	const int count = static_cast<int>(shopItems_.size());
	for (int i = 0; i < count; ++i)
	{
		auto& ro = shopItems_[i];
		if (!ro) continue;

		const Matrix4x4 wvp = shopWvps_[i] * viewProj;
		ro->CopyBufferData(shopBindings_[i].wvpVsCbvIndex, &wvp, sizeof(Matrix4x4));
		ro->CopyBufferData(shopBindings_[i].colorPsCbvIndex, &shopColors_[i], sizeof(Vector4));
	}
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	drawBackPack_->Draw(cmdObject);

	for (auto& ro : shopItems_)
	{
		if (ro)
		{
			ro->Draw(cmdObject);
		}
	}
}

void BackPack::DrawImGui()
{
	ImGui::Begin("BackPack");


	ImGui::End();


}

void BackPack::InitializeShopItems_(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager)
{
	constexpr int kShopNum = 5;

	shopItems_.clear();
	shopItems_.reserve(kShopNum);

	shopBindings_.clear();
	shopBindings_.resize(kShopNum);

	shopWvps_.assign(kShopNum, Matrix4x4::Identity());
	shopColors_.assign(kShopNum, Vector4(1, 1, 1, 1));

	// ショップ表示のワールド行列
	std::array<Matrix4x4, kShopNum> worlds{};
	for (int i = 0; i < kShopNum; ++i)
	{
		Transform t{};
		t.scale = { 0.7f, 0.7f, 0.7f };

		// 奇数の時は左、偶数の時は右に配置
		if (i % 2 == 0)
		{
			t.position.x = -4.0f;
		}
		else
		{
			t.position.x = -8.0f;
		}
		t.position.y = 0.5f;
		t.position.z = float(i) * 1.5f;
		worlds[i] = Matrix::MakeAffineMatrix(t.scale, t.rotate, t.position);
	}

	for (int i = 0; i < kShopNum; ++i)
	{
		const Item& item = itemManager->GetItem(i);
		const int modelID = item.modelID;

		if (modelID < 0)
		{
			// modelID 未解決なら cube フォールバック
			const int fallback = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");
			const auto mdl = modelManager->GetNodeModelData(fallback);
			const auto dd = drawDataManager->GetDrawData(mdl.drawDataIndex);

			auto ro = std::make_unique<SHEngine::RenderObject>();
			ro->Initialize();
			ro->SetDrawData(dd);
			ro->SetUseTexture(false);

			ro->psoConfig_.vs = "Simple.VS.hlsl";
			ro->psoConfig_.ps = "Color.PS.hlsl";

			shopBindings_[i].wvpVsCbvIndex = ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
			shopBindings_[i].colorPsCbvIndex = ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

			shopColors_[i] = item.color;

			shopItems_.push_back(std::move(ro));
			continue;
		}

		const auto modelData = modelManager->GetNodeModelData(modelID);
		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

		auto ro = std::make_unique<SHEngine::RenderObject>("ShopItem_" + std::to_string(i));
		ro->Initialize();
		ro->SetDrawData(drawData);
		ro->SetUseTexture(false);

		ro->psoConfig_.vs = "Simple.VS.hlsl";
		ro->psoConfig_.ps = "Color.PS.hlsl";

		shopBindings_[i].wvpVsCbvIndex = ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
		shopBindings_[i].colorPsCbvIndex = ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

		shopColors_[i] = item.color;

		shopItems_.push_back(std::move(ro));
	}

	// world は Update 内で viewProj と掛ける必要があるため、ここで shopWvps_ に「world」を一旦入れておく
	// Update で wvp=world*viewProj に上書きする
	for (int i = 0; i < kShopNum; ++i)
	{
		shopWvps_[i] = worlds[i];
	}
}







DrawBackPack::DrawBackPack()
{
	lockedUnavailableGrid_ = std::make_unique<SHEngine::RenderObject>("DrawBackPackLockedUnavailable");
	lockedAvailableGrid_ = std::make_unique<SHEngine::RenderObject>("DrawBackPackLockedAvailable");
	unlockedEmptyGrid_ = std::make_unique<SHEngine::RenderObject>("DrawBackPackUnlockedEmpty");
}

DrawBackPack::~DrawBackPack()
{}

void DrawBackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager)
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

	// worlds の予約
	lockedUnavailableWorlds_.reserve(maxInstance);
	lockedAvailableWorlds_.reserve(maxInstance);
	unlockedEmptyWorlds_.reserve(maxInstance);
}

void DrawBackPack::UpdateRenderObject(
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


void DrawBackPack::Update(const Matrix4x4& viewProj, const std::vector<std::vector<std::unique_ptr<BackPackGrid>>>& grids)
{
	lockedUnavailableWorlds_.clear();
	lockedAvailableWorlds_.clear();
	unlockedEmptyWorlds_.clear();

	for (size_t r = 0; r < GameConstants::kBackPackRowNum; ++r)
	{
		for (size_t c = 0; c < GameConstants::kBackPackColNum; ++c)
		{
			const auto& g = grids[r][c];

			Matrix4x4 world = Matrix::MakeAffineMatrix(
				g->transform_.scale,
				g->transform_.rotate,
				g->transform_.position
			);

			switch (g->GetState())
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
				break;
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

void DrawBackPack::Draw(SHEngine::Command::Object* cmdObject)
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

void DrawBackPack::DrawImGui()
{
	ImGui::Begin("DrawBackPack");


	ImGui::End();


}