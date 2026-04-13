#include "TrailEditorScene.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <numbers>
#include "03_YokoScene/YokoScene.h"
#include "PrticleEditorScene.h"

using namespace SHEngine;

namespace
{
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

		const Vector4 color = { 1,1,1,1 };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &textureIndex, sizeof(int));

		return ro;
	}

	std::unique_ptr<RenderObject> CreateDataRO()
	{
		auto ro = std::make_unique<RenderObject>();
		ro->Initialize();

		ro->psoConfig_.vs = "Game/Field.VS.hlsl";
		ro->psoConfig_.ps = "Game/Field.PS.hlsl";
		ro->SetUseTexture(true);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		ro->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		const Vector4 color = { 1,1,1,1 };
		ro->CopyBufferData(1, &color, sizeof(Vector4));

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
	// カメラ初期化
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 3.0f, -10.0f });
	camera_->Initialize(input_);

	// ワールドグリッド初期化
	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	// 編集データ初期化
	Reset(TrailType::RibbonTrail);

	// "Assets/Model"以下のモデルをリストアップしてmodelDataList_作成
	BuildModelList();
	// "Assets/Json/Trail"以下のjsonをリストアップしてJsonList_作成
	BuildJsonList();

	// RenderObject作成
	modelRender_ = CreateDataRO();
	int modelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
	auto modelData = modelManager_->GetNodeModelData(modelHandle);
	marker[0] = CreateTexturedModelRO(drawDataManager_, modelData, 0);
	marker[1] = CreateTexturedModelRO(drawDataManager_, modelData, 0);

	// モデルトランスフォーム初期化
	modelTransform_.position = { 0.0f, 0.0f, 0.0f };
	modelTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	modelTransform_.scale = { 1.0f, 1.0f, 1.0f };

	// 初期モデルを選択
	SelectModel(0);

	RebuildTrail();
}

// 編集データ初期化
void TrailEditorScene::Reset(TrailType type)
{
	currentType_ = type;

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.texturePath.c_str(), _TRUNCATE);

	trailConfig_ = Trail::Config{};

	// ribbon
	ribbonPreset_ = RibbonTrailConfig{};
	ribbonPreset_.cfg = trailConfig_;

	// shock
	shockPreset_ = ShockwaveRingConfig{};
	shockPreset_.cfg = trailConfig_;
}

// "Assets/Model/"以下のモデルをリストアップしてmodelDataList_作成。武器追従トレイルにしか使わない機能だから簡易的でよい
void TrailEditorScene::BuildModelList()
{
	//const char* kWeaponDir = "Assets/Model/Item/Weapon";
	const char* kFilePath = "Assets/Model";
	std::error_code ec;
	if (!std::filesystem::exists(kFilePath, ec))
	{
		return;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(kFilePath))
	{
		// is_directory() = それがフォルダかファイルか
		if (!entry.is_directory()) continue;

		// 子フォルダがある場合はスキップ
		bool hasChildDir = false;
		for (const auto& c : std::filesystem::directory_iterator(entry.path()))
		{
			if (c.is_directory()) { hasChildDir = true; break; }
		}
		if (hasChildDir) continue;

		// ここまで来たentryは最下層のフォルダ
		auto data = std::make_unique<DrawDataUnit>();
		data->modelPath = entry.path().generic_string();
		data->name = entry.path().filename().generic_string();
		data->modelIndex = modelManager_->LoadModel(data->modelPath);
		modelDataList_.push_back(std::move(data));
	}
}
// "Assets/Json/Trail"以下のjsonをリストアップしてJsonList_作成
void TrailEditorScene::BuildJsonList()
{
	const char* kFilePath = "Assets/Json/Trail";
	std::error_code ec;
	if (!std::filesystem::exists(kFilePath, ec))
	{
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(kFilePath))
	{
		// is_directory() = それがフォルダかファイルか
		if (entry.is_directory()) continue;

		JsonList_.push_back(entry.path().filename().generic_string());
	}
}


// 選択モデルを変更
void TrailEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= int(modelDataList_.size())) return;
	selectedModelIndex_ = index;
	auto modelData = modelManager_->GetNodeModelData(modelDataList_[index]->modelIndex);
	const auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	modelRender_->SetDrawData(drawData);
	modelDataList_[index]->textureIndex = modelData.materials[modelData.materialIndex.front()].textureIndex;
}

// Trailの再構築
void TrailEditorScene::RebuildTrail()
{
	trail_.Clear();
	trail_.Initialize(textureManager_, &commonData_->trailPresetDataBank);
	for (const auto& name : activeTrailNameList_)
	{
		trail_.Add(name);
	}
	trail_.RegisterToDrawer(&commonData_->trailDrawer);

	editingTrail_.Clear();
	editingTrail_.Initialize(textureManager_);
	editingTrail_.SetConfig(trailConfig_);
	commonData_->trailDrawer.Register(&editingTrail_);
}

// データ保存
void TrailEditorScene::SaveTrailData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == TrailType::RibbonTrail)
	{
		commonData_->trailPresetDataBank.Save(presetNameBuf_, trailConfig_, ribbonPreset_);
	}
	else if (currentType_ == TrailType::ShockwaveRing)
	{
		commonData_->trailPresetDataBank.Save(presetNameBuf_, trailConfig_, shockPreset_);
	}
}
// データ読み込み
void TrailEditorScene::LoadTrailData()
{
	if (presetNameBuf_[0] == '\0') return;

	TrailPresetVariant var{};

	var = commonData_->trailPresetDataBank.Get(presetNameBuf_);

	if (std::holds_alternative<RibbonTrailConfig>(var))
	{
		currentType_ = TrailType::RibbonTrail;
		ribbonPreset_ = std::get<RibbonTrailConfig>(var);
		trailConfig_ = ribbonPreset_.cfg;
		std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
		strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.texturePath.c_str(), _TRUNCATE);

	}
	else if (std::holds_alternative<ShockwaveRingConfig>(var))
	{
		currentType_ = TrailType::ShockwaveRing;
		shockPreset_ = std::get<ShockwaveRingConfig>(var);
		trailConfig_ = shockPreset_.cfg;
		std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
		strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), trailConfig_.texturePath.c_str(), _TRUNCATE);
	}

	requestRebuildTrail_ = true;
}


void TrailEditorScene::DrawConfigUI_()
{
#ifdef USE_IMGUI

	requestRebuildTrail_ |= ImGui::DragInt("cfg.maxSegments", &trailConfig_.maxSegments, 1.0f, 1, 512);
	requestRebuildTrail_ |= ImGui::DragFloat("cfg.lifeTime", &trailConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("cfg.minDistance", &trailConfig_.minDistance, 0.001f, 0.0f, 10.0f);

	if (ImGui::InputText("cfg.texturePath", texturePathBuf_, sizeof(texturePathBuf_)))
	{
		trailConfig_.texturePath = texturePathBuf_;
		requestRebuildTrail_ = true;
	}

	requestRebuildTrail_ |= ImGui::ColorEdit4("cfg.color", &trailConfig_.color.x);

#endif
}

void TrailEditorScene::DrawRibbonUI_()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("リボン型");
	requestRebuildTrail_ |= ImGui::DragFloat3("ribbon.originLocal", &ribbonPreset_.originLocal.x, 0.01f);
	requestRebuildTrail_ |= ImGui::DragFloat3("ribbon.tipLocal", &ribbonPreset_.tipLocal.x, 0.01f);

#endif
}
void TrailEditorScene::DrawShockwaveUI_()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("Shockwave Ring");
	requestRebuildTrail_ |= ImGui::DragInt("shock.segments", &shockPreset_.segments, 1.0f, 3, 512);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.duration", &shockPreset_.duration, 0.01f, 0.01f, 10.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.radiusStart", &shockPreset_.radiusStart, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.radiusEnd", &shockPreset_.radiusEnd, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.thickness", &shockPreset_.thickness, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.noiseAmp", &shockPreset_.noiseAmp, 0.01f, 0.0f, 100.0f);
	requestRebuildTrail_ |= ImGui::DragFloat("shock.noiseFreq", &shockPreset_.noiseFreq, 0.01f, 0.0f, 100.0f);

#endif
}

void TrailEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("TrailEditor");

	// 現在編集中のプリセット名
	if (ImGui::TreeNode("編集中トレイル名"))
	{
		ImGui::InputText("##tinnko", presetNameBuf_, sizeof(presetNameBuf_));

		ImGui::TreePop();
	}
	// 現在表示している(編集は出来ない)プリセット名
	if (ImGui::TreeNode("表示中トレイル名"))
	{
		// ⇩BeginListBoxに変更し、smallButtonもつけ、[削除]を追加描画リストから外せるようにする
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)activeTrailNameList_.size(); ++i)
			{
				ImGui::PushID(i); // 行ごとにIDを分ける
				// 行全体を横並びにする
				ImGui::BeginGroup();
				// 左側：Selectable
				bool selected = ImGui::Selectable(activeTrailNameList_[i].c_str(), false, 0, ImVec2(200, 0));
				// 右側：ボタン
				ImGui::SameLine();
				if (ImGui::SmallButton("削除"))
				{
					activeTrailNameList_.erase(activeTrailNameList_.begin() + i);
					requestRebuildTrail_ = true;
				}
				ImGui::EndGroup();
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	// モデル選択
	if (ImGui::TreeNode("表示モデル選択"))
	{
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)modelDataList_.size(); ++i)
			{
				const bool selected = (i == selectedModelIndex_);
				if (ImGui::Selectable(modelDataList_[i]->name.c_str(), selected))
				{
					SelectModel(i);
					requestRebuildTrail_ = true;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}
	// トレイル選択
	if (ImGui::TreeNode("表示・編集トレイル選択"))
	{
		if (ImGui::BeginListBox("##sdubjn;", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)JsonList_.size(); ++i)
			{
				ImGui::PushID(i); // 行ごとにIDを分ける

				// 行全体を横並びにする
				ImGui::BeginGroup();

				// 左側：Selectable
				bool selected = ImGui::Selectable(JsonList_[i].c_str(), false, 0, ImVec2(200, 0));

				// 右側：ボタン
				ImGui::SameLine();

				if (ImGui::SmallButton("編集"))
				{
					strncpy_s(presetNameBuf_, sizeof(presetNameBuf_), JsonList_[i].c_str(), _TRUNCATE);
					LoadTrailData();
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("描画"))
				{
					activeTrailNameList_.push_back(JsonList_[i]);
					requestRebuildTrail_ = true;
				}

				ImGui::EndGroup();

				// 単押し処理（Selectable）
				if (selected)
				{
					activeTrailNameList_.push_back(JsonList_[i]);
					requestRebuildTrail_ = true;
				}

				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	ImGui::SeparatorText("Config");

	DrawConfigUI_();

	DrawRibbonUI_();

	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		SaveTrailData();
	}
	ImGui::Checkbox("モデル描画", &isModelDraw_);
	ImGui::Checkbox("マーカー描画", &isMarkerDraw_);

	ImGui::End();



	ImGui::Begin("modelTransform");
	ImGui::DragFloat3("position", &modelTransform_.position.x, 0.01f);
	ImGui::DragFloat3("rotate", &modelTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &modelTransform_.scale.x, 0.01f);
	ImGui::End();

#endif
}

void TrailEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
{
	if (selectedModelIndex_ < 0) return;

	modelWorld_ = MakeWorld(modelTransform_);

	// モデル
	{
		const Matrix4x4 wvp = modelWorld_ * vpMatrix;
		const Vector4 color = { 1,1,1,1 };

		modelRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		modelRender_->CopyBufferData(1, &color, sizeof(Vector4));
		modelRender_->CopyBufferData(2, &modelDataList_[selectedModelIndex_]->textureIndex, sizeof(int));
	}

	markerPos[0] = ribbonPreset_.originLocal * modelWorld_;
	markerPos[1] = ribbonPreset_.tipLocal * modelWorld_;

	// marker origin
	{
		for (int i = 0; i < 2; ++i)
		{
			Transform tr{};
			tr.position = markerPos[i];
			tr.scale = { 0.5f, 0.5f, 0.5f };
			const Matrix4x4 wvp = MakeWorld(tr) * vpMatrix;
			const Vector4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
			const int texIndex = 0;

			marker[i]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
			marker[i]->CopyBufferData(1, &color, sizeof(Vector4));
			marker[i]->CopyBufferData(2, &texIndex, sizeof(int));
		}
	}
}

std::unique_ptr<IScene> TrailEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	// ワールドグリッド更新
	grid_->Update(Vector3(0.0f, 0.0f, 0.0f), vp);

	// トレイル再生成
	if (requestRebuildTrail_)
	{
		requestRebuildTrail_ = false;
		RebuildTrail();
	}

	// モデル・マーカー更新
	UpdateRenders(vp);

	// Trail更新
	trail_.SetModelWorld(modelWorld_);
	trail_.Update(dt);
	editingTrail_.PushSegment(markerPos[0], markerPos[1]);
	editingTrail_.Update(dt);

	// Zキーで切り替え
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z))
	{
		return std::make_unique<YokoScene>();
	}

	return nullptr;
}

void TrailEditorScene::Draw()
{
	if (selectedModelIndex_ < 0) return;

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);

	if (isModelDraw_)modelRender_->Draw(cmdObj);

	if (isMarkerDraw_)
	{
		for (int i = 0; i < 2; ++i)
		{
			marker[i]->Draw(cmdObj);
		}
	}

	commonData_->trailDrawer.Draw(cmdObj, camera_->GetVPMatrix());


	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	DrawImGui();
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}