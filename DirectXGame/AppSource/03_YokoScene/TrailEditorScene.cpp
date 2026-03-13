#include "TrailEditorScene.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <cstring>

using namespace SHEngine;

namespace
{
	constexpr const char* kWeaponDir = "Assets/Model/Item/Weapon";

	std::string GetLeafName(const std::string& path)
	{
		std::filesystem::path p(path);
		return p.filename().string();
	}

	std::unique_ptr<RenderObject> CreateTexturedModelRO(
		DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		int textureIndex,
		const char* debugName)
	{
		auto ro = std::make_unique<RenderObject>(debugName);
		ro->Initialize();

		ro->psoConfig_.vs = "Game/Field.VS.hlsl";
		ro->psoConfig_.ps = "Game/Field.PS.hlsl";
		ro->SetUseTexture(true);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		ro->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		// 初期値
		const Vector4 color = { 1,1,1,1 };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &textureIndex, sizeof(int));

		return ro;
	}

	std::unique_ptr<RenderObject> CreateColorMarkerRO(
		DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		const char* debugName)
	{
		auto ro = std::make_unique<RenderObject>(debugName);
		ro->Initialize();

		ro->psoConfig_.vs = "Simple.VS.hlsl";
		ro->psoConfig_.ps = "Color.PS.hlsl";
		ro->SetUseTexture(false);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		return ro;
	}

	Matrix4x4 MakeWorld(const Transform& tr)
	{
		return Matrix::MakeAffineMatrix(tr.scale, tr.rotate, tr.position);
	}
}

void TrailEditorScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 3.0f, -10.0f });
	camera_->Initialize(input_);

	// マーカー用Cube（EngineResourceに確実にある）
	markerModelHandle_ = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
	markerModelData_ = modelManager_->GetNodeModelData(markerModelHandle_);

	// Trail初期値
	trailCfg_ = Trail::Config{};
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailCfg_.defaultTexturePath.c_str(), _TRUNCATE);

	// モデルリスト
	BuildModelList();
	if (!models_.empty())
	{
		SelectModel(0);
	}
}

void TrailEditorScene::BuildModelList()
{
	models_.clear();

	std::error_code ec;
	if (!std::filesystem::exists(kWeaponDir, ec))
	{
		return;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(kWeaponDir))
	{
		if (!entry.is_directory()) continue;

		// 末端フォルダ（子ディレクトリを持たない）だけ対象
		bool hasChildDir = false;
		for (const auto& c : std::filesystem::directory_iterator(entry.path()))
		{
			if (c.is_directory())
			{
				hasChildDir = true;
				break;
			}
		}
		if (hasChildDir) continue;

		ModelEntry m{};
		m.path = entry.path().generic_string();
		m.name = entry.path().filename().generic_string();
		models_.push_back(std::move(m));
	}

	std::sort(models_.begin(), models_.end(), [](const ModelEntry& a, const ModelEntry& b)
		{
			return a.name < b.name;
		});
}

void TrailEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= (int)models_.size()) return;

	selectedModelIndex_ = index;
	auto& m = models_[selectedModelIndex_];

	m.modelHandle = modelManager_->LoadModel(m.path);
	m.modelData = modelManager_->GetNodeModelData(m.modelHandle);

	m.textureIndex = 0;
	m.textureIndex = m.modelData.materials[m.modelData.materialIndex.front()].textureIndex;
	modelTextureIndex_ = m.textureIndex;

	// modelRender作り直し
	CreateModelRender();
	CreateMarkerRenders();

	// 保存済みがあればロード
	LoadTrailData();

	// Trail作り直し
	RebuildTrail();
}

// ターゲットオブジェクトのRenderObjectを作成
void TrailEditorScene::CreateModelRender()
{
	if (selectedModelIndex_ < 0) return;
	auto& m = models_[selectedModelIndex_];

	modelRender_ = CreateTexturedModelRO(drawDataManager_, m.modelData, m.textureIndex, "TrailEditor_Model");

	modelTransform_.position = { 0.0f, 0.0f, 0.0f };
	modelTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	modelTransform_.scale = { 1.0f, 1.0f, 1.0f };
}

// トレイルマーカーのRenderObjectを作成
void TrailEditorScene::CreateMarkerRenders()
{
	markerOriginRender_ = CreateColorMarkerRO(drawDataManager_, markerModelData_, "TrailEditor_MarkerOrigin");
	markerTipRender_ = CreateColorMarkerRO(drawDataManager_, markerModelData_, "TrailEditor_MarkerTip");
}

// Trailの初期化
void TrailEditorScene::RebuildTrail()
{
	trail_.Initialize(drawDataManager_, textureManager_, trailCfg_);
	trail_.SetTexturePath(trailCfg_.defaultTexturePath);
	trail_.Clear();
}

// Jsonにトレイルデータ保存
void TrailEditorScene::SaveTrailData()
{
	if (selectedModelIndex_ < 0) return;

	const std::string modelName = models_[selectedModelIndex_].name;
	const std::string fileBaseName = modelName + "TrailData";

	json_.Boot(fileBaseName);

	// points
	{
		Vector3 o = originLocal_;
		Vector3 t = tipLocal_;
		json_.Add("originLocal", o);
		json_.Add("tipLocal", t);
	}

	// config
	{
		int maxSeg = trailCfg_.maxSegments;
		float life = trailCfg_.lifeTime;
		float minDist = trailCfg_.minDistance;

		Vector4 cN = trailCfg_.colorNormal;
		Vector4 cA = trailCfg_.colorAdd;

		bool dn = trailCfg_.drawNormal;
		bool da = trailCfg_.drawAdd;

		std::string tex = trailCfg_.defaultTexturePath;

		json_.Add("maxSegments", maxSeg);
		json_.Add("lifeTime", life);
		json_.Add("minDistance", minDist);

		json_.Add("colorNormal", cN);
		json_.Add("colorAdd", cA);

		json_.Add("drawNormal", dn);
		json_.Add("drawAdd", da);

		json_.Add("defaultTexturePath", tex);
	}

	json_.Save();
}

void TrailEditorScene::LoadTrailData()
{
	if (selectedModelIndex_ < 0) return;

	const std::string modelName = models_[selectedModelIndex_].name;
	const std::string fileBaseName = modelName + "TrailData";

	json_.Boot(fileBaseName);

	try { originLocal_ = json_.Get<Vector3>("originLocal"); }
	catch (...) {}
	try { tipLocal_ = json_.Get<Vector3>("tipLocal"); }
	catch (...) {}

	try { trailCfg_.maxSegments = json_.Get<int>("maxSegments"); }
	catch (...) {}
	try { trailCfg_.lifeTime = json_.Get<float>("lifeTime"); }
	catch (...) {}
	try { trailCfg_.minDistance = json_.Get<float>("minDistance"); }
	catch (...) {}

	try { trailCfg_.colorNormal = json_.Get<Vector4>("colorNormal"); }
	catch (...) {}
	try { trailCfg_.colorAdd = json_.Get<Vector4>("colorAdd"); }
	catch (...) {}

	try { trailCfg_.drawNormal = json_.Get<bool>("drawNormal"); }
	catch (...) {}
	try { trailCfg_.drawAdd = json_.Get<bool>("drawAdd"); }
	catch (...) {}

	try { trailCfg_.defaultTexturePath = json_.Get<std::string>("defaultTexturePath"); }
	catch (...) {}

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailCfg_.defaultTexturePath.c_str(), _TRUNCATE);
}

void TrailEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
{
	lastVp_ = vpMatrix;

	// model WVP
	{
		const Matrix4x4 w = MakeWorld(modelTransform_);
		modelWvp_ = w * vpMatrix;

		const Vector4 color = { 1,1,1,1 };
		modelRender_->CopyBufferData(0, &modelWvp_, sizeof(Matrix4x4));
		modelRender_->CopyBufferData(1, &color, sizeof(Vector4));
		modelRender_->CopyBufferData(2, &modelTextureIndex_, sizeof(int));
	}

	// marker WS（ローカル→ワールド）
	const Matrix4x4 modelWorld = MakeWorld(modelTransform_);
	originWS_ = originLocal_ * modelWorld;
	tipWS_ = tipLocal_ * modelWorld;

	// marker Origin
	{
		Transform tr{};
		tr.position = originWS_;
		tr.rotate = { 0,0,0 };
		tr.scale = { markerScale_, markerScale_, markerScale_ };

		Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
		markerOriginRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		markerOriginRender_->CopyBufferData(1, &markerOriginColor_, sizeof(Vector4));
	}

	// marker Tip
	{
		Transform tr{};
		tr.position = tipWS_;
		tr.rotate = { 0,0,0 };
		tr.scale = { markerScale_, markerScale_, markerScale_ };

		Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
		markerTipRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		markerTipRender_->CopyBufferData(1, &markerTipColor_, sizeof(Vector4));
	}
}

void TrailEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("TrailEditor");

	// モデル選択
	if (ImGui::BeginListBox("Weapon Models"))
	{
		for (int i = 0; i < (int)models_.size(); ++i)
		{
			const bool selected = (i == selectedModelIndex_);
			if (ImGui::Selectable(models_[i].name.c_str(), selected))
			{
				SelectModel(i);
			}
		}
		ImGui::EndListBox();
	}

	if (selectedModelIndex_ >= 0)
	{
		ImGui::SeparatorText("Point (Local)");
		requestRebuildTrail_ |= ImGui::DragFloat3("Origin Local", &originLocal_.x, 0.01f);
		requestRebuildTrail_ |= ImGui::DragFloat3("Tip Local", &tipLocal_.x, 0.01f);

		ImGui::SeparatorText("Marker");
		ImGui::DragFloat("Marker Scale", &markerScale_, 0.01f, 0.01f, 10.0f);

		ImGui::SeparatorText("Trail Config");
		requestRebuildTrail_ |= ImGui::DragInt("maxSegments", &trailCfg_.maxSegments, 1.0f, 1, 512);
		requestRebuildTrail_ |= ImGui::DragFloat("lifeTime", &trailCfg_.lifeTime, 0.01f, 0.001f, 10.0f);
		requestRebuildTrail_ |= ImGui::DragFloat("minDistance", &trailCfg_.minDistance, 0.001f, 0.0f, 10.0f);

		requestRebuildTrail_ |= ImGui::ColorEdit4("colorNormal", &trailCfg_.colorNormal.x);
		requestRebuildTrail_ |= ImGui::ColorEdit4("colorAdd", &trailCfg_.colorAdd.x);

		requestRebuildTrail_ |= ImGui::Checkbox("drawNormal", &trailCfg_.drawNormal);
		requestRebuildTrail_ |= ImGui::Checkbox("drawAdd", &trailCfg_.drawAdd);

		// texture path
		if (ImGui::InputText("defaultTexturePath", texturePathBuf_, sizeof(texturePathBuf_)))
		{
			trailCfg_.defaultTexturePath = texturePathBuf_;
			requestRebuildTrail_ = true;
		}

		ImGui::Separator();
		ImGui::Checkbox("Emit Trail", &emitTrail_);
		if (ImGui::Button("Clear Trail"))
		{
			trail_.Clear();
		}

		if (ImGui::Button("Save"))
		{
			SaveTrailData();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			LoadTrailData();
			requestRebuildTrail_ = true;
		}

		ImGui::Text("Save File: Assets/Json/%sTrailData.json", models_[selectedModelIndex_].name.c_str());
	}

	ImGui::End();
#endif
}

std::unique_ptr<IScene> TrailEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	camera_->Update();

	DrawImGui();

	// Trail再構築
	if (requestRebuildTrail_)
	{
		requestRebuildTrail_ = false;
		RebuildTrail();
	}

	const Matrix4x4 vp = camera_->GetVPMatrix();
	UpdateRenders(vp);

	// Trail更新（WSの2点から）
	if (emitTrail_)
	{
		trail_.PushSegment(originWS_, tipWS_);
	}
	trail_.Update(dt, vp);

	return nullptr;
}

void TrailEditorScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	if (modelRender_) modelRender_->Draw(cmdObj);
	if (markerOriginRender_) markerOriginRender_->Draw(cmdObj);
	if (markerTipRender_) markerTipRender_->Draw(cmdObj);

	trail_.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}