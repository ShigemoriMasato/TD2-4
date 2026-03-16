#include "TrailEditorScene.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <numbers>

using namespace SHEngine;

namespace
{

	std::string GetLeafName(const std::string& path)
	{
		std::filesystem::path p(path);
		return p.filename().string();
	}

	std::unique_ptr<RenderObject> CreateTexturedModelRO(
		DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		int textureIndex)
	{
		auto ro = std::make_unique<RenderObject>();
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

	// マーカー用Cube
	int markerModelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
	markerModelData_ = modelManager_->GetNodeModelData(markerModelHandle);

	// Trail初期値
	trailConfig_ = Trail::Config{};

	int i = 0;
	for (i = 0; i < trailConfig_.defaultTexturePath.size(); ++i)
	{
		texturePathBuf_[i] = trailConfig_.defaultTexturePath[i];
	}
	texturePathBuf_[i] = '\0';

	// モデルリスト作成
	BuildModelList();

	// 全モデルのRenderObject作成
	CreateModelRender();
	// マーカーのRenderObject作成
	CreateMarkerRenders();

	// Trail作り直し
	RebuildTrail();

	// 最初のモデルを選択
	SelectModel(0);

	// 保存済みがあればロード
	LoadTrailData();
}

// Assets/Model/Item/Weapon配下モデルファイルをリストアップ
void TrailEditorScene::BuildModelList()
{
	modelRenders_.clear();

	const char* kWeaponDir = "Assets/Model/Item/Weapon";

	// kWeaponDirフォルダの存在確認
	std::error_code ec;
	if (!std::filesystem::exists(kWeaponDir, ec))
	{
		return;
	}

	// kWeaponDir配下の全フォルダを走査
	for (const auto& entry : std::filesystem::recursive_directory_iterator(kWeaponDir))
	{
		// entryがフォルダでなければスキップ
		if (!entry.is_directory()) continue;

		// 末端フォルダだけ対象
		bool hasChildDir = false;
		for (const auto& c : std::filesystem::directory_iterator(entry.path()))
		{
			// さらに下がいれば非末端判定
			if (c.is_directory())
			{
				hasChildDir = true;
				break;
			}
		}
		// 走査して非末端ならスキップ
		if (hasChildDir) continue;

		/// 末端じゃなければ追加
		std::unique_ptr<editDataUnit> data = std::make_unique<editDataUnit>();
		// モデルデータを取得
		data->modelHandle = modelManager_->LoadModel(entry.path().generic_string());
		// 表示用の名前をセット（例: Sword）
		data->name = entry.path().filename().generic_string();
		modelRenders_.push_back(std::move(data));
	}
}

// selectedModelIndex_を更新
void TrailEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= int(modelRenders_.size())) return;

	selectedModelIndex_ = index;
}

// ターゲットオブジェクトのRenderObjectを作成
void TrailEditorScene::CreateModelRender()
{
	for (auto& data : modelRenders_)
	{
		auto modelData = modelManager_->GetNodeModelData(data->modelHandle);
		// テクスチャインデックスを取得
		data->textureIndex = modelData.materials[modelData.materialIndex.front()].textureIndex;
		data->render = CreateTexturedModelRO(drawDataManager_, modelData, data->textureIndex);
	}

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
	trail_.Initialize(drawDataManager_, textureManager_, trailConfig_);
	trail_.SetTexturePath(trailConfig_.defaultTexturePath);
	trail_.Clear();
}

// Jsonにトレイルデータ保存
void TrailEditorScene::SaveTrailData()
{
	const std::string modelName = modelRenders_[selectedModelIndex_]->name;
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
		int maxSeg = trailConfig_.maxSegments;
		float life = trailConfig_.lifeTime;
		float minDist = trailConfig_.minDistance;

		Vector4 cN = trailConfig_.colorNormal;
		Vector4 cA = trailConfig_.colorAdd;

		bool dn = trailConfig_.drawNormal;
		bool da = trailConfig_.drawAdd;

		std::string tex = trailConfig_.defaultTexturePath;

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
// Jsonからトレイルデータ読み込み
void TrailEditorScene::LoadTrailData()
{
	const std::string modelName = modelRenders_[selectedModelIndex_]->name;
	const std::string fileBaseName = modelName + "TrailData";

	json_.Boot(fileBaseName);

	try { originLocal_ = json_.Get<Vector3>("originLocal"); }
	catch (...) {}
	try { tipLocal_ = json_.Get<Vector3>("tipLocal"); }
	catch (...) {}

	try { trailConfig_.maxSegments = json_.Get<int>("maxSegments"); }
	catch (...) {}
	try { trailConfig_.lifeTime = json_.Get<float>("lifeTime"); }
	catch (...) {}
	try { trailConfig_.minDistance = json_.Get<float>("minDistance"); }
	catch (...) {}

	try { trailConfig_.colorNormal = json_.Get<Vector4>("colorNormal"); }
	catch (...) {}
	try { trailConfig_.colorAdd = json_.Get<Vector4>("colorAdd"); }
	catch (...) {}

	try { trailConfig_.drawNormal = json_.Get<bool>("drawNormal"); }
	catch (...) {}
	try { trailConfig_.drawAdd = json_.Get<bool>("drawAdd"); }
	catch (...) {}

	try { trailConfig_.defaultTexturePath = json_.Get<std::string>("defaultTexturePath"); }
	catch (...) {}

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.defaultTexturePath.c_str(), _TRUNCATE);
}

void TrailEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
{
	// model WVP
	{
		const Matrix4x4 w = MakeWorld(modelTransform_);
		modelWvp_ = w * vpMatrix;

		const Vector4 color = { 1,1,1,1 };
		modelRenders_[selectedModelIndex_]->render->CopyBufferData(0, &modelWvp_, sizeof(Matrix4x4));
		modelRenders_[selectedModelIndex_]->render->CopyBufferData(1, &color, sizeof(Vector4));
		modelRenders_[selectedModelIndex_]->render->CopyBufferData(2, &modelRenders_[selectedModelIndex_]->textureIndex, sizeof(int));
	}

	// marker WS（ローカル→ワールド）
	const Matrix4x4 modelWorld = MakeWorld(modelTransform_);
	originWS_ = originLocal_ * modelWorld;
	tipWS_ = tipLocal_ * modelWorld;

	// marker Origin
	{
		Transform tr{};
		tr.position = originWS_;
		tr.scale = { 0.5f, 0.5f, 0.5f };

		Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
		markerOriginRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		Vector4 color = Vector4{ 1.0f, 0.2f, 0.2f, 1.0f };
		markerOriginRender_->CopyBufferData(1, &color, sizeof(Vector4));
	}

	// marker Tip
	{
		Transform tr{};
		tr.position = tipWS_;
		tr.scale = { 0.5f, 0.5f, 0.5f };

		Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
		markerTipRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		Vector4 color = Vector4{ 0.2f, 1.0f, 0.2f, 1.0f };
		markerTipRender_->CopyBufferData(1, &color, sizeof(Vector4));
	}
}

void TrailEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("TrailEditor");

	// モデル選択
	if (ImGui::BeginListBox("Weapon Models"))
	{
		for (int i = 0; i < modelRenders_.size(); ++i)
		{
			const bool selected = (i == selectedModelIndex_);
			if (ImGui::Selectable(modelRenders_[i]->name.c_str(), selected))
			{
				SelectModel(i);
			}
		}
		ImGui::EndListBox();
	}

	if (selectedModelIndex_ >= 0)
	{
		ImGui::SeparatorText("Point");
		requestRebuildTrail_ |= ImGui::DragFloat3("Origin", &originLocal_.x, 0.01f);
		requestRebuildTrail_ |= ImGui::DragFloat3("Tip", &tipLocal_.x, 0.01f);

		ImGui::SeparatorText("Trail Config");
		requestRebuildTrail_ |= ImGui::DragInt("maxSegments", &trailConfig_.maxSegments, 1.0f, 1, 512);
		requestRebuildTrail_ |= ImGui::DragFloat("lifeTime", &trailConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
		requestRebuildTrail_ |= ImGui::DragFloat("minDistance", &trailConfig_.minDistance, 0.001f, 0.0f, 10.0f);

		requestRebuildTrail_ |= ImGui::ColorEdit4("colorNormal", &trailConfig_.colorNormal.x);
		requestRebuildTrail_ |= ImGui::ColorEdit4("colorAdd", &trailConfig_.colorAdd.x);

		requestRebuildTrail_ |= ImGui::Checkbox("drawNormal", &trailConfig_.drawNormal);
		requestRebuildTrail_ |= ImGui::Checkbox("drawAdd", &trailConfig_.drawAdd);

		// texture path
		if (ImGui::InputText("defaultTexturePath", texturePathBuf_, sizeof(texturePathBuf_)))
		{
			trailConfig_.defaultTexturePath = texturePathBuf_;
			requestRebuildTrail_ = true;
		}

		ImGui::Separator();
		ImGui::Checkbox("Emit Trail", &emitTrail_);

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

		ImGui::Text("Save File: Assets/Json/%sTrailData.json", modelRenders_[selectedModelIndex_]->name.c_str());
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

	modelRenders_[selectedModelIndex_]->render->Draw(cmdObj);
	markerOriginRender_->Draw(cmdObj);
	markerTipRender_->Draw(cmdObj);

	trail_.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}