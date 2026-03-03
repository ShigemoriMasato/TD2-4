#include "Shop.h"
#include "GameObject/Item/ItemManager.h"

Shop::Shop()
{
}

Shop::~Shop()
{
}



void Shop::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, ItemManager* itemManager, CommonData* commonData, SHEngine::Input* input)
{
	itemManager_ = itemManager;
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	commonData_ = commonData;
	input_ = input;


	auto [w, h] = commonData_->mainWindow.first->GetWindowSize();
	screenRaycaster_ = std::make_unique<ScreenRaycaster>(static_cast<float>(w), static_cast<float>(h));

	// デバッグライン描画初期化
	debugLineDrawer_ = std::make_unique<PrimitiveLineDrawer>();
	debugLineDrawer_->Initialize(drawDataManager_, 256);

	for (int i = 0; i < lineupSum; ++i)
	{
		LineupItemData data;
		data.renderObject = std::make_unique<SHEngine::RenderObject>();
		data.renderObject->Initialize();
		data.renderObject->psoConfig_.vs = "Simple.VS.hlsl";
		data.renderObject->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		data.renderObject->psoConfig_.ps = "Color.PS.hlsl";
		data.renderObject->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER);
		if (i % 2 == 0) data.InitPos.x = -5.0f;
		else data.InitPos.x = -10.0f;
		data.InitPos.y = 0.5f;
		data.InitPos.z = 13.0f - (float(i) * 3.0f);
		data.hoverPos = data.InitPos;
		lineupItems_.push_back(std::move(data));
	}

	RandomPickup();
}

void Shop::RandomPickup()
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

bool Shop::CanPlaceHaveItem()
{
	haveItem_.reset();
	return false;
}

void Shop::Update(const Matrix4x4& viewProj)
{
	// デバッグライン描画の内容をクリア。気にしなくてよい。
	debugLineDrawer_->Clear();

	// マウスの状態を取得
	auto cur = input_->GetMouseButtonState();
	auto pre = input_->GetPreMouseButtonState();

	// 左クリックの状態を取得
	const bool leftDown = (cur[0] & 0x80) != 0;                 // 押している
	const bool leftTrigger = ((cur[0] & 0x80) != 0) && ((pre[0] & 0x80) == 0); // 押した瞬間
	const bool leftRelease = ((cur[0] & 0x80) == 0) && ((pre[0] & 0x80) != 0); // 離した瞬間

	// マウスレイ取得
	screenRaycaster_->SetInverseVP(viewProj.Inverse());
	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	mouseRay_ = screenRaycaster_->ScreenToRay(cursorPos.x, cursorPos.y);

	// ラインナップ商品の更新
	for (int i = 0; i < lineupSum; ++i)
	{
		// 商品を取得
		auto& data = lineupItems_[i];

		// アイテムの位置を計算
		PlaneXZ worldPlane = data.item.boundyPlane.Translate(data.hoverPos);

		// 商品とマウスレイが衝突している && 左クリックした && なにも持っていない
		if (IsCollision(mouseRay_, worldPlane) && leftTrigger && !haveItem_.has_value())
		{
			data.isHover = true;
			haveItem_ = data.item;
		}

		// ホバー状態ならレイ上に移動
		if (data.isHover)
		{
			// ホバー位置をマウスレイ
			auto mousePosOnXZ = mouseRay_.GetXZ(0.0f);
			if (mousePosOnXZ.has_value())
			{
				data.hoverPos = mousePosOnXZ.value();
			}

			// クリックを離した時バックパックに置けるか判定
			if (leftRelease)
			{
				// 置けるなら置いてホバー状態解除
				if (CanPlaceHaveItem())
				{
					data.InitPos = data.hoverPos;
				}
				else
				{
				}
				data.isHover = false;
			}
		}
		// ホバー状態でなければ初期位置に移動
		else
		{
			// 初期位置に向かう
			data.hoverPos = data.InitPos;
		}

		// 衝突判定エリア描画
		debugLineDrawer_->AddPlaneXZ(worldPlane, 0xff0000ff);
	}
	for (int i = 0; i < lineupSum; ++i)
	{
		// 商品を取得
		auto& data = lineupItems_[i];

		// CBV更新
		Matrix4x4 worldMatrix = Matrix::MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), data.hoverPos);
		data.wvp = worldMatrix * viewProj;
		data.renderObject->CopyBufferData(0, &lineupItems_[i].wvp, sizeof(Matrix4x4));
		data.renderObject->CopyBufferData(1, &lineupItems_[i].color, sizeof(Vector4));
	}

	// デバッグライン描画の更新
	debugLineDrawer_->Update(viewProj);
}

void Shop::Draw(SHEngine::Command::Object* cmdObject)
{
	for (auto& itemRender : lineupItems_)
	{
		itemRender.renderObject->Draw(cmdObject);
	}
	debugLineDrawer_->Draw(cmdObject);
}

void Shop::DrawImGui()
{
#ifdef USE_IMGUI
	Vector2 cursorPos = commonData_->keyManager->GetCursorPos();
	for (int i = 0; i < lineupSum; ++i)
	{
		auto& data = lineupItems_[i];
		ImGui::DragFloat3(("Item " + std::to_string(i)).c_str(), &data.hoverPos.x, 0.1f);
	}
#endif // USE_IMGUI
}

bool Shop::IsCollision(const Ray& r, const AABB& aabb)
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

std::optional<Vector3> Shop::IntersectRayAABB(const Ray& ray, const AABB& box)
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

bool Shop::IsCollision(Ray& r, const PlaneXZ& plane)
{
	auto intersectPointOpt = r.GetXZ(plane.center.y);
	if (!intersectPointOpt.has_value()) return false;

	Vector3 intersectPoint = intersectPointOpt.value();
	float halfWidth = plane.width / 2.0f;
	float halfHeight = plane.height / 2.0f;
	return (intersectPoint.x >= plane.center.x - halfWidth &&
		intersectPoint.x <= plane.center.x + halfWidth &&
		intersectPoint.z >= plane.center.z - halfHeight &&
		intersectPoint.z <= plane.center.z + halfHeight);
}

