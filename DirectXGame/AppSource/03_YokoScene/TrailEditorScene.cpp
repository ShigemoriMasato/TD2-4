#include "TrailEditorScene.h"
#include <imgui/imgui.h>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <numbers>
#include "03_YokoScene/YokoScene.h"
#include <Utility/DataStructures.h>
#include "PrticleEditorScene.h"
#include <Utility/SearchFile.h>

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
		ro->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		const Vector4 color = { 1,1,1,1 };
		const DirectionalLight dirLight = { {1,1,1,1}, {0,-1,0}, 1.0f };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &textureIndex, sizeof(int));
		ro->CopyBufferData(3, &dirLight, sizeof(DirectionalLight));

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
		ro->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");

		const Vector4 color = { 1,1,1,1 };
		const DirectionalLight dirLight = { {1,1,1,1}, {0,-1,0}, 1.0f };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(3, &dirLight, sizeof(DirectionalLight));

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
	Reset();

	// "Assets/Model"以下のモデルをリストアップしてmodelDataList_作成
	BuildModelList();
	// "Assets/Texture"以下のテクスチャをリストアップしてTextureList_作成
	BuildTextureList();
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

	snprintf(presetNameBuf_, sizeof(presetNameBuf_), "%s", JsonList_[0].c_str());
	RebuildEditTrailByJson();
}

// 編集データ初期化
void TrailEditorScene::Reset()
{
	trailConfig_ = TrailConfig{};

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
	modelList_.clear();

	const char* kFilePath = "Assets/Model";
	auto modelFileNames = SearchDirectoryPathsAddChild(kFilePath);
	modelList_ = modelFileNames;
	std::sort(modelList_.begin(), modelList_.end());
	return;


	std::error_code ec;
	if (!std::filesystem::exists(kFilePath, ec))
	{
		return;
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(kFilePath))
	{
		// is_directory() = それがフォルダかファイルか
		if (!entry.is_directory()) continue;

		modelList_.push_back(entry.path().filename().generic_string());
	}
	std::sort(modelList_.begin(), modelList_.end());
}
// "Assets/Texture/"以下のテクスチャをリストアップしてTextureList_作成。
void TrailEditorScene::BuildTextureList()
{
	textureList_.clear();

	const char* kFilePath = "Assets/Texture";

	auto textureFileNames = SearchFilePathsAddChild(kFilePath, ".png");
	textureList_ = textureFileNames;
	std::sort(textureList_.begin(), textureList_.end());
	return;
}
// "Assets/Json/Trail"以下のjsonをリストアップしてJsonList_作成
void TrailEditorScene::BuildJsonList()
{
	JsonList_.clear();

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
	std::sort(JsonList_.begin(), JsonList_.end());
}



// 描画トレイルのみ再生成
void TrailEditorScene::RebuildDrawTrail()
{
	drawingTrail_.Clear();
	drawingTrail_.Initialize(textureManager_, commonData_);
	for (const auto& name : activeTrailNameList_)
	{
		drawingTrail_.Add(name);
	}
}
// 編集トレイルのみ再生成。Jsonの内容で再構築
void TrailEditorScene::RebuildEditTrailByJson()
{
	editingTrail_.Clear();
	editingTrail_.Initialize(textureManager_, commonData_);
	// 編集プリセットを一つだけ追加する
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingTrail_.Add(presetNameBuf_);
	// 追加したプリセットのConfigをセット
	TrailPresetVariant presetVar = editingTrail_.GetConfig(slot);
	if (std::holds_alternative<RibbonTrailConfig>(presetVar))
	{
		ribbonPreset_ = std::get<RibbonTrailConfig>(presetVar);
		trailConfig_ = ribbonPreset_.cfg;
		currentType_ = TrailType::RibbonTrail;
	}
	else if (std::holds_alternative<ShockwaveRingConfig>(presetVar))
	{

	}
}
// 編集トレイルのみ再生成。現在のConfigで再構築
void TrailEditorScene::RebuildEditTrailByCurrentConfig()
{
	editingTrail_.Clear();
	editingTrail_.Initialize(textureManager_, commonData_);
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingTrail_.Add(presetNameBuf_);

	if (currentType_ == TrailType::RibbonTrail)
	{
		ribbonPreset_.cfg = trailConfig_;
		editingTrail_.SetConfig(slot, ribbonPreset_);
	}
	else if (currentType_ == TrailType::ShockwaveRing)
	{
		shockPreset_.cfg = trailConfig_;
		editingTrail_.SetConfig(slot, shockPreset_);
	}
}


// データ保存
void TrailEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == TrailType::RibbonTrail)
	{
		ribbonPreset_.cfg = trailConfig_;
		commonData_->trailPresetDataBank.Save(presetNameBuf_, trailConfig_, ribbonPreset_);
	}
	else if (currentType_ == TrailType::ShockwaveRing)
	{
		shockPreset_.cfg = trailConfig_;
		commonData_->trailPresetDataBank.Save(presetNameBuf_, trailConfig_, shockPreset_);
	}
}

// データ読み込み
void TrailEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	TrailPresetVariant var{};
	var = commonData_->trailPresetDataBank.Get(presetNameBuf_);

	if (std::holds_alternative<RibbonTrailConfig>(var))
	{
		currentType_ = TrailType::RibbonTrail;
		ribbonPreset_ = std::get<RibbonTrailConfig>(var);
		trailConfig_ = ribbonPreset_.cfg;
	}
	else if (std::holds_alternative<ShockwaveRingConfig>(var))
	{
		currentType_ = TrailType::ShockwaveRing;
		shockPreset_ = std::get<ShockwaveRingConfig>(var);
		trailConfig_ = shockPreset_.cfg;
	}
}



// 選択モデルを変更
void TrailEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= int(modelList_.size())) return;
	selectedModelIndex_ = index;
	int modelIndex = modelManager_->LoadModel("Assets/Model/" + modelList_[index]);
	auto modelData = modelManager_->GetNodeModelData(modelIndex);
	const auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	modelRender_->SetDrawData(drawData);
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
					requestRebuildDrawingTrail_ = true;
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
			for (int i = 0; i < (int)modelList_.size(); ++i)
			{
				const bool selected = (i == selectedModelIndex_);
				if (ImGui::Selectable(modelList_[i].c_str(), selected))
				{
					SelectModel(i);
					requestRebuildDrawingTrail_ = true;
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
					requestRebuildEditingTrailByJson_ = true;
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("描画"))
				{
					activeTrailNameList_.push_back(JsonList_[i]);
					requestRebuildDrawingTrail_ = true;
				}

				ImGui::EndGroup();

				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	ImGui::SeparatorText("共通Config");
	DrawImGui_Config();

	ImGui::SeparatorText("固有Config");
	switch (currentType_)
	{
	case TrailType::RibbonTrail:
		DrawImGui_Config_Ribbon();
		break;
	case TrailType::ShockwaveRing:
		DrawImGui_Config_Shockwave();
		break;
	case TrailType::None:
		break;
	default:
		break;
	}

	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		SaveData();
		BuildJsonList();
	}
	ImGui::Checkbox("モデル描画", &isModelDraw_);
	ImGui::Checkbox("マーカー描画", &isMarkerDraw_);

	ImGui::End();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("modelTransform");
	ImGui::DragFloat3("position", &modelTransform_.position.x, 0.01f);
	ImGui::DragFloat3("rotate", &modelTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &modelTransform_.scale.x, 0.01f);
	ImGui::End();

#endif
}
void TrailEditorScene::DrawImGui_Config()
{
#ifdef USE_IMGUI

	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragInt("cfg.maxSegments", &trailConfig_.maxSegments, 1.0f, 1, 512);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("cfg.lifeTime", &trailConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("cfg.minDistance", &trailConfig_.minDistance, 0.001f, 0.0f, 10.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::ColorEdit4("cfg.color", &trailConfig_.color.x);

	ImGui::Text("cfg.texturePath %s", trailConfig_.texturePath.c_str());
	if (ImGui::TreeNode("テクスチャ選択"))
	{
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)textureList_.size(); ++i)
			{
				if (ImGui::Selectable(textureList_[i].c_str(), false))
				{
					// texturePathBuf_に選択したテクスチャのパスをセット
					trailConfig_.texturePath = textureList_[i];
					requestRebuildEditingTrailByCurrentConfig_ = true;
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}


#endif
}
void TrailEditorScene::DrawImGui_Config_Ribbon()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("リボン型");
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat3("ribbon.originLocal", &ribbonPreset_.originLocal.x, 0.01f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat3("ribbon.tipLocal", &ribbonPreset_.tipLocal.x, 0.01f);

#endif
}
void TrailEditorScene::DrawImGui_Config_Shockwave()
{
#ifdef USE_IMGUI

	ImGui::SeparatorText("Shockwave Ring");
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragInt("shock.segments", &shockPreset_.segments, 1.0f, 3, 512);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.duration", &shockPreset_.duration, 0.01f, 0.01f, 10.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.radiusStart", &shockPreset_.radiusStart, 0.01f, 0.0f, 100.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.radiusEnd", &shockPreset_.radiusEnd, 0.01f, 0.0f, 100.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.thickness", &shockPreset_.thickness, 0.01f, 0.0f, 100.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.noiseAmp", &shockPreset_.noiseAmp, 0.01f, 0.0f, 100.0f);
	requestRebuildEditingTrailByCurrentConfig_ |= ImGui::DragFloat("shock.noiseFreq", &shockPreset_.noiseFreq, 0.01f, 0.0f, 100.0f);

#endif
}


void TrailEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
{
	//if (selectedModelIndex_ < 0) return;

	modelTransform_.rotate.x += 0.05f;
	modelTransform_.rotate.y += 0.02f;
	modelWorld_ = MakeWorld(modelTransform_);

	// モデル
	{
		const Matrix4x4 wvp = modelWorld_ * vpMatrix;
		const Vector4 color = { 1,1,1,1 };
		const int texIndex = 10;
		const DirectionalLight dirLight = { {1,1,1,1}, {0,-1,0}, 1.0f };

		modelRender_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
		modelRender_->CopyBufferData(1, &color, sizeof(Vector4));
		//modelRender_->CopyBufferData(2, &modelDataList_[selectedModelIndex_]->textureIndex, sizeof(int));
		modelRender_->CopyBufferData(2, &texIndex, sizeof(int));
		modelRender_->CopyBufferData(3, &dirLight, sizeof(DirectionalLight));
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
			const DirectionalLight dirLight = { {1,1,1,1}, {0,-1,0}, 1.0f };

			marker[i]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
			marker[i]->CopyBufferData(1, &color, sizeof(Vector4));
			marker[i]->CopyBufferData(2, &texIndex, sizeof(int));
			marker[i]->CopyBufferData(3, &dirLight, sizeof(DirectionalLight));
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
	if (requestRebuildDrawingTrail_)
	{
		requestRebuildDrawingTrail_ = false;
		RebuildDrawTrail();
	}
	if (requestRebuildEditingTrailByJson_)
	{
		requestRebuildEditingTrailByJson_ = false;
		RebuildEditTrailByJson();
	}
	if (requestRebuildEditingTrailByCurrentConfig_)
	{
		requestRebuildEditingTrailByCurrentConfig_ = false;
		RebuildEditTrailByCurrentConfig();
	}

	// モデル・マーカー更新
	UpdateRenders(vp);

	// Trail更新
	editingTrail_.SetModelWorld(modelWorld_);
	editingTrail_.Update(dt);
	drawingTrail_.SetModelWorld(modelWorld_);
	drawingTrail_.Update(dt);

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

	cmdObj->SetRenderTarget(display->GetDisplay());

	grid_->Draw(cmdObj);

	//if (isModelDraw_)modelRender_->Draw(cmdObj);

	if (isMarkerDraw_)
	{
		for (int i = 0; i < 2; ++i)
		{
			marker[i]->Draw(cmdObj);
		}
	}

	drawingTrail_.Draw();
	editingTrail_.Draw();

	commonData_->trailDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	display->PostDraw(cmdObj);

	cmdObj->SetRenderTarget(window->GetCurrentDisplay());

#ifdef USE_IMGUI
	DrawImGui();
	display->DrawImGui();
#endif

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}