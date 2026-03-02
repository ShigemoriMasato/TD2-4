#include "BackPack.h"
#include <Utility/Matrix.h>
#include "GameObject/Item/ItemManager.h"



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









ItemLineup::ItemLineup()
{
}

ItemLineup::~ItemLineup()
{}

void ItemLineup::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData)
{
	itemManager_ = itemManager;
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	commonData_ = commonData;


	auto [w, h] = commonData_->mainWindow.first->GetWindowSize();
	screenRaycaster_ = std::make_unique<ScreenRaycaster>(static_cast<float>(w), static_cast<float>(h));


	for (int i = 0; i < lineupSum; ++i)
	{
		LineupItemData data;
		data.renderObject = std::make_unique<SHEngine::RenderObject>();
		data.renderObject->Initialize();
		data.renderObject->psoConfig_.vs = "Simple.VS.hlsl";
		data.renderObject->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		data.renderObject->psoConfig_.ps = "Color.PS.hlsl";
		data.renderObject->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER);
		if (i % 2 == 0) data.InitPos.x = -4.0f;
		else data.InitPos.x = -8.0f;
		data.InitPos.y = 0.5f;
		data.InitPos.z = float(i) * 15.0f;
		lineupItems_.push_back(std::move(data));
	}

	RandomPickup();
}

void ItemLineup::RandomPickup()
{
	for (int i = 0; i < lineupSum; ++i)
	{
		// アイテムをランダムに選択してラインナップに追加
		int randomIndex = rand();
		Item pickItem = itemManager_->GetItem(randomIndex);

		const auto modelData = modelManager_->GetNodeModelData(pickItem.modelID);
		const auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
		lineupItems_[i].renderObject->SetDrawData(drawData);
		lineupItems_[i].item = pickItem;
		lineupItems_[i].color = pickItem.color;
	}
}

void ItemLineup::Update(const Matrix4x4& viewProj)
{
	screenRaycaster_->SetInverseVP(viewProj.Inverse());
	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	mouseRay_ = screenRaycaster_->ScreenToRay(cursorPos.x, cursorPos.y);

	for (int i = 0; i < lineupSum; ++i)
	{
		auto& data = lineupItems_[i];

		Matrix4x4 worldMatrix = Matrix::MakeAffineMatrix(Vector3(1.0f,1.0f,1.0f), Vector3(0.0f,0.0f,0.0f), data.hoverPos);
		AABB worldAABB = data.item.aabb.Transform(worldMatrix);

		if (IsCollision(mouseRay_, worldAABB))
		{
			data.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		}
		else
		{
			data.color = data.item.color;
		}

		data.wvp = worldMatrix * viewProj;

		data.renderObject->CopyBufferData(0, &lineupItems_[i].wvp, sizeof(Matrix4x4));
		data.renderObject->CopyBufferData(1, &lineupItems_[i].color, sizeof(Vector4));
	}
}

void ItemLineup::Draw(SHEngine::Command::Object* cmdObject)
{
	for (auto& itemRender : lineupItems_)
	{
		itemRender.renderObject->Draw(cmdObject);
	}
}

void ItemLineup::DrawImGui()
{
	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	ImGui::Text("Cursor Position: (%.1f, %.1f)", cursorPos.x, cursorPos.y);
}

bool ItemLineup::IsCollision(const Ray& r, const AABB& aabb)
{
	float tmin = (aabb.min.x - r.origin.x) / r.direction.x;
	float tmax = (aabb.max.x - r.origin.x) / r.direction.x;
	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (aabb.min.y - r.origin.y) / r.direction.y;
	float tymax = (aabb.max.y - r.origin.y) / r.direction.y;
	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (aabb.min.z - r.origin.z) / r.direction.z;
	float tzmax = (aabb.max.z - r.origin.z) / r.direction.z;
	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	return true;
}

std::optional<Vector3> ItemLineup::IntersectRayAABB(const Ray& ray, const AABB& box)
{
	const float EPSILON = 1e-8f;
	float tmin = 0.0f;
	float tmax = std::numeric_limits<float>::infinity();

	// スラブ（軸ごとの射線区間）を更新するラムダ
	auto slab = [&](float origin, float dir, float bmin, float bmax) -> bool
		{
			// X方向のベクトルが０に限りなく近ければ、X平面で見た時レイは点のように見える。
			if (std::fabs(dir) < EPSILON)
			{
				// その点がAABBのＸ軸から見た平面内だったら衝突しているといえる。Ｙ軸Ｚ軸についても同様のことであってくれ
				return (origin >= bmin && origin <= bmax);
			}


			float invD = 1.0f / dir;
			float t1 = (bmin - origin) * invD;
			float t2 = (bmax - origin) * invD;
			// 
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
			return (tmax >= tmin);
		};

	if (!slab(ray.origin.x, ray.direction.x, box.min.x, box.max.x)) return std::nullopt;
	if (!slab(ray.origin.y, ray.direction.y, box.min.y, box.max.y)) return std::nullopt;
	if (!slab(ray.origin.z, ray.direction.z, box.min.z, box.max.z)) return std::nullopt;

	// レイがボックスの後方にしかない場合は衝突なし
	if (tmax < 0.0f) return std::nullopt;

	// tmin が正なら最初の交差、負ならボックス内スタート → tmax を使う
	float tHit = (tmin >= 0.0f) ? tmin : tmax;
	return ray.origin + ray.direction * tHit;
}






BackPack::BackPack()
{
	drawBackPack_ = std::make_unique<DrawBackPack>();

	itemLineup_ = std::make_unique<ItemLineup>();

	for (int i = 0; i < GameConstants::kBackPackRowNum; ++i)
	{
		std::vector<std::unique_ptr<BackPackGrid>> row;
		for (int j = 0; j < GameConstants::kBackPackColNum; ++j)
		{
			row.push_back(std::make_unique<BackPackGrid>());
		}
		grids_.push_back(std::move(row));
	}
}

BackPack::~BackPack() {};

void BackPack::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData)
{
	drawBackPack_->Initialize(modelManager, drawDataManager);

	itemLineup_->Initialize(modelManager, drawDataManager, itemManager, commonData);

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

	itemLineup_->Update(viewProj);
}

void BackPack::Draw(SHEngine::Command::Object* cmdObject)
{
	drawBackPack_->Draw(cmdObject);

	itemLineup_->Draw(cmdObject);
}

void BackPack::DrawImGui()
{
	ImGui::Begin("BackPack");

	itemLineup_->DrawImGui();

	ImGui::End();
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