#include "PrticleEditorScene.h"
#include <Utility/DataStructures.h>
#include "03_YokoScene/YokoScene.h"

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

void PrticleEditorScene::Initialize()
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
	// "Assets/Json/Particle"以下のjsonをリストアップしてJsonList_作成
	BuildJsonList();


	snprintf(presetNameBuf_, sizeof(presetNameBuf_), "%s", JsonList_[0].c_str());
	RebuildEditParticleByJson();
}

// 編集データ初期化
void PrticleEditorScene::Reset()
{
	particleConfig_ = ParticleConfig{};

	// GoToTarget
	goToTargetConfig_ = GoToTargetConfig{};
	goToTargetConfig_.cfg = particleConfig_;

	// B_S_R_T_C
	b_S_R_T_C_Config_ = B_S_R_T_C_Config{};
	b_S_R_T_C_Config_.cfg = particleConfig_;
}

// "Assets/Model/"以下のモデルをリストアップしてmodelDataList_作成。
void PrticleEditorScene::BuildModelList()
{
	modelList_.clear();

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
	
		modelList_.push_back(entry.path().filename().generic_string());
	}
	std::sort(modelList_.begin(), modelList_.end());
}
// "Assets/Texture/"以下のテクスチャをリストアップしてTextureList_作成。
void PrticleEditorScene::BuildTextureList()
{
	textureList_.clear();

	const char* kFilePath = "Assets/Texture";
	std::error_code ec;
	if (!std::filesystem::exists(kFilePath, ec))
	{
		return;
	}
	for (const auto& entry : std::filesystem::recursive_directory_iterator(kFilePath))
	{
		// is_directory() = それがフォルダかファイルか
		if (entry.is_directory()) continue;

		textureList_.push_back(entry.path().filename().generic_string());
	}
	std::sort(textureList_.begin(), textureList_.end());
}
// "Assets/Json/Particle"以下のjsonをリストアップしてJsonList_作成
void PrticleEditorScene::BuildJsonList()
{
	JsonList_.clear();

	const char* kFilePath = "Assets/Json/Particle";
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

// 描画パーティクルのみ再生成
void PrticleEditorScene::RebuildDrawParticle()
{
	drawingParticle_.Clear();
	drawingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	for (const auto& name : activeParticleNameList_)
	{
		drawingParticle_.Add(name);
	}
}
// 編集中のParticleの再構築。Jsonの内容で再構築
void PrticleEditorScene::RebuildEditParticleByJson()
{
	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	// 編集プリセットを一つだけ追加する
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingParticle_.Add(presetNameBuf_);
	// 追加したプリセットのConfigをセット
	ParticlePresetVariant presetVar = editingParticle_.GetConfig(slot);
	if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		goToTargetConfig_ = std::get<GoToTargetConfig>(presetVar);
		particleConfig_ = goToTargetConfig_.cfg;
		currentType_ = ParticleType::GoToTarget;
	}
	else if (std::holds_alternative<B_S_R_T_C_Config>(presetVar))
	{
		b_S_R_T_C_Config_ = std::get<B_S_R_T_C_Config>(presetVar);
		particleConfig_ = b_S_R_T_C_Config_.cfg;
		currentType_ = ParticleType::B_S_R_T_C;
	}
}
// 編集中のParticleの再構築。現在のConfigで再構築
void PrticleEditorScene::RebuildEditParticleByCurrentConfig()
{
	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingParticle_.Add(presetNameBuf_);

	if (currentType_ == ParticleType::GoToTarget)
	{
		goToTargetConfig_.cfg = particleConfig_;
		editingParticle_.SetConfig(slot, goToTargetConfig_);
	}
	else if (currentType_ == ParticleType::B_S_R_T_C)
	{
		b_S_R_T_C_Config_.cfg = particleConfig_;
		editingParticle_.SetConfig(slot, b_S_R_T_C_Config_);
	}
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == ParticleType::GoToTarget)
	{
		goToTargetConfig_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, goToTargetConfig_);
	}
	else if (currentType_ == ParticleType::B_S_R_T_C)
	{
		b_S_R_T_C_Config_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, b_S_R_T_C_Config_);
	}
}

// データ読み込み
void PrticleEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	ParticlePresetVariant var{};
	var = commonData_->particlePresetDataBank.Get(presetNameBuf_);

	if (std::holds_alternative<GoToTargetConfig>(var))
	{
		currentType_ = ParticleType::GoToTarget;
		goToTargetConfig_ = std::get<GoToTargetConfig>(var);
		particleConfig_ = goToTargetConfig_.cfg;
	}
	else if (std::holds_alternative<B_S_R_T_C_Config>(var))
	{
		currentType_ = ParticleType::B_S_R_T_C;
		b_S_R_T_C_Config_ = std::get<B_S_R_T_C_Config>(var);
		particleConfig_ = b_S_R_T_C_Config_.cfg;
	}
}



void PrticleEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("ParticleEditor");

	// 現在編集中のプリセット名
	if (ImGui::TreeNode("編集中パーティクル名"))
	{
		ImGui::InputText("##tinnko", presetNameBuf_, sizeof(presetNameBuf_));

		ImGui::TreePop();
	}
	// 現在表示している(編集は出来ない)プリセット名
	if (ImGui::TreeNode("表示中パーティクル名"))
	{
		// [削除]描画リストから外せるようにする
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)activeParticleNameList_.size(); ++i)
			{
				ImGui::PushID(i); // 行ごとにIDを分ける
				// 行全体を横並びにする
				ImGui::BeginGroup();
				// 左側：Selectable
				ImGui::Selectable(activeParticleNameList_[i].c_str(), false, 0, ImVec2(200, 0));
				// ボタン
				ImGui::SameLine();
				if (ImGui::SmallButton("削除"))
				{
					activeParticleNameList_.erase(activeParticleNameList_.begin() + i);
					requestRebuildDrawingParticle_ = true;
				}
				ImGui::EndGroup();
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	// パーティクル選択
	if (ImGui::TreeNode("表示・編集パーティクル選択"))
	{
		if (ImGui::BeginListBox("##sdubjn;", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)JsonList_.size(); ++i)
			{
				ImGui::PushID(i); // 行ごとにIDを分ける

				// 行全体を横並びにする
				ImGui::BeginGroup();
				
				// 左側：Selectable
				ImGui::Selectable(JsonList_[i].c_str(), false, 0, ImVec2(200, 0));

				ImGui::SameLine();

				if (ImGui::SmallButton("編集"))
				{
					snprintf(presetNameBuf_, sizeof(presetNameBuf_), "%s", JsonList_[i].c_str());
					// EditerのデータをJsonのデータに合わせたい
					requestRebuildEditingParticleByJson_ = true;
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("描画"))
				{
					activeParticleNameList_.push_back(JsonList_[i]);
					// 描画リストに追加
					requestRebuildDrawingParticle_ = true;
				}

				ImGui::EndGroup();

				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	// type
	{
		int t = int(currentType_);
		const char* items[] = 
		{
			"GoToTarget", 
			"B_S_R_T_C"
		};
		if (ImGui::Combo("type", &t, items, IM_ARRAYSIZE(items)))
		{
			currentType_ = ParticleType(t);
			requestRebuildEditingParticleByCurrentConfig_ = true;
		}
	}

	ImGui::SeparatorText("共通Config");
	DrawImGui_Config();

	ImGui::SeparatorText("固有Config");
	switch (currentType_)
	{
	case ParticleType::GoToTarget:
		DrawImGui_Config_GoToTarget();
		break;
	case ParticleType::B_S_R_T_C:
		DrawImGui_Config_B_S_R_T_C();
		break;
	case ParticleType::None:

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

	ImGui::End();

	ImGui::Begin("worldTransform_");

	ImGui::DragFloat3("position", &worldTransform_.position.x, 0.01f);
	ImGui::DragFloat3("rotate", &worldTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &worldTransform_.scale.x, 0.01f);

	ImGui::End();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

#endif
}

// DrawImGui_Config系でconfigが変更された場合はrequestRebuildEditingParticleByCurrentConfig_フラグを立て、editingParticle_をconfifに合わせて再構築する。
void PrticleEditorScene::DrawImGui_Config()
{
#ifdef USE_IMGUI
	if (ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f))
	{
		requestRebuildEditingParticleByCurrentConfig_ = true;
	}
	if (ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f))
	{
		requestRebuildEditingParticleByCurrentConfig_ = true;
	}
	if (ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000))
	{
		requestRebuildEditingParticleByCurrentConfig_ = true;
	}
	if (ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.01f, 10.0f))
	{
		requestRebuildEditingParticleByCurrentConfig_ = true;
	}
	if (ImGui::Checkbox("cfg.isBillboard", &particleConfig_.isBillboard_))
	{
		requestRebuildEditingParticleByCurrentConfig_ = true;
	}

	ImGui::Text("cfg.texturePath %s", particleConfig_.texturePath.c_str());
	if (ImGui::TreeNode("テクスチャ選択"))
	{
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)textureList_.size(); ++i)
			{
				if (ImGui::Selectable(textureList_[i].c_str(), false))
				{
					// texturePathBuf_に選択したテクスチャのパスをセット
					particleConfig_.texturePath = textureList_[i];
					requestRebuildEditingParticleByCurrentConfig_ = true;
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
	ImGui::Text("cfg.modelPath %s", particleConfig_.modelPath.c_str());
	if (ImGui::TreeNode("表示モデル選択"))
	{
		if (ImGui::BeginListBox("##sihpo;dj", ImVec2(-FLT_MIN - 100, 100)))
		{
			for (int i = 0; i < (int)modelList_.size(); ++i)
			{
				if (ImGui::Selectable(modelList_[i].c_str(), false))
				{
					particleConfig_.modelPath = modelList_[i];
					requestRebuildEditingParticleByCurrentConfig_ = true;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}
#endif
}
void PrticleEditorScene::DrawImGui_Config_GoToTarget()
{}
void PrticleEditorScene::DrawImGui_Config_B_S_R_T_C()
{
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.scale.isRandomVal", &b_S_R_T_C_Config_.scale.value.isRandom);
		if (b_S_R_T_C_Config_.scale.value.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.rand.min", &b_S_R_T_C_Config_.scale.value.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.rand.max", &b_S_R_T_C_Config_.scale.value.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.scale.value.randomRange_max.x < b_S_R_T_C_Config_.scale.value.randomRange_min.x)
			{
				b_S_R_T_C_Config_.scale.value.randomRange_max.x = b_S_R_T_C_Config_.scale.value.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.scale.value.randomRange_max.y < b_S_R_T_C_Config_.scale.value.randomRange_min.y)
			{
				b_S_R_T_C_Config_.scale.value.randomRange_max.y = b_S_R_T_C_Config_.scale.value.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.scale.value.randomRange_max.z < b_S_R_T_C_Config_.scale.value.randomRange_min.z)
			{
				b_S_R_T_C_Config_.scale.value.randomRange_max.z = b_S_R_T_C_Config_.scale.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.scale.val", &b_S_R_T_C_Config_.scale.value.baseValue.x, 0.01f);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.scale.isRandomVel", &b_S_R_T_C_Config_.scale.velocity.isRandom);
		if (b_S_R_T_C_Config_.scale.velocity.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.vel.rand.min", &b_S_R_T_C_Config_.scale.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.vel.rand.max", &b_S_R_T_C_Config_.scale.velocity.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.scale.velocity.randomRange_max.x < b_S_R_T_C_Config_.scale.velocity.randomRange_min.x)
			{
				b_S_R_T_C_Config_.scale.velocity.randomRange_max.x = b_S_R_T_C_Config_.scale.velocity.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.scale.velocity.randomRange_max.y < b_S_R_T_C_Config_.scale.velocity.randomRange_min.y)
			{
				b_S_R_T_C_Config_.scale.velocity.randomRange_max.y = b_S_R_T_C_Config_.scale.velocity.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.scale.velocity.randomRange_max.z < b_S_R_T_C_Config_.scale.velocity.randomRange_min.z)
			{
				b_S_R_T_C_Config_.scale.velocity.randomRange_max.z = b_S_R_T_C_Config_.scale.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.scale.vel", &b_S_R_T_C_Config_.scale.velocity.baseValue.x, 0.01f);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.scale.isRandomAcc", &b_S_R_T_C_Config_.scale.acceleration.isRandom);
		if (b_S_R_T_C_Config_.scale.acceleration.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.acc.rand.min", &b_S_R_T_C_Config_.scale.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("scale.acc.rand.max", &b_S_R_T_C_Config_.scale.acceleration.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.scale.acceleration.randomRange_max.x < b_S_R_T_C_Config_.scale.acceleration.randomRange_min.x)
			{
				b_S_R_T_C_Config_.scale.acceleration.randomRange_max.x = b_S_R_T_C_Config_.scale.acceleration.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.scale.acceleration.randomRange_max.y < b_S_R_T_C_Config_.scale.acceleration.randomRange_min.y)
			{
				b_S_R_T_C_Config_.scale.acceleration.randomRange_max.y = b_S_R_T_C_Config_.scale.acceleration.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.scale.acceleration.randomRange_max.z < b_S_R_T_C_Config_.scale.acceleration.randomRange_min.z)
			{
				b_S_R_T_C_Config_.scale.acceleration.randomRange_max.z = b_S_R_T_C_Config_.scale.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.scale.acc", &b_S_R_T_C_Config_.scale.acceleration.baseValue.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("rotate"))
	{
		if (!particleConfig_.isBillboard_)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomVal", &b_S_R_T_C_Config_.rotate.value.isRandom);
			if (b_S_R_T_C_Config_.rotate.value.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.rand.min", &b_S_R_T_C_Config_.rotate.value.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.rand.max", &b_S_R_T_C_Config_.rotate.value.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.value.randomRange_max.x < b_S_R_T_C_Config_.rotate.value.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.value.randomRange_max.x = b_S_R_T_C_Config_.rotate.value.randomRange_min.x;
				}
				if (b_S_R_T_C_Config_.rotate.value.randomRange_max.y < b_S_R_T_C_Config_.rotate.value.randomRange_min.y)
				{
					b_S_R_T_C_Config_.rotate.value.randomRange_max.y = b_S_R_T_C_Config_.rotate.value.randomRange_min.y;
				}
				if (b_S_R_T_C_Config_.rotate.value.randomRange_max.z < b_S_R_T_C_Config_.rotate.value.randomRange_min.z)
				{
					b_S_R_T_C_Config_.rotate.value.randomRange_max.z = b_S_R_T_C_Config_.rotate.value.randomRange_min.z;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.rotate.val", &b_S_R_T_C_Config_.rotate.value.baseValue.x, 0.01f);
			}

			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomVel", &b_S_R_T_C_Config_.rotate.velocity.isRandom);
			if (b_S_R_T_C_Config_.rotate.velocity.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.vel.rand.min", &b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.vel.rand.max", &b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x < b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x = b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x;
				}
				if (b_S_R_T_C_Config_.rotate.velocity.randomRange_max.y < b_S_R_T_C_Config_.rotate.velocity.randomRange_min.y)
				{
					b_S_R_T_C_Config_.rotate.velocity.randomRange_max.y = b_S_R_T_C_Config_.rotate.velocity.randomRange_min.y;
				}
				if (b_S_R_T_C_Config_.rotate.velocity.randomRange_max.z < b_S_R_T_C_Config_.rotate.velocity.randomRange_min.z)
				{
					b_S_R_T_C_Config_.rotate.velocity.randomRange_max.z = b_S_R_T_C_Config_.rotate.velocity.randomRange_min.z;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.rotate.vel", &b_S_R_T_C_Config_.rotate.velocity.baseValue.x, 0.01f);
			}

			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &b_S_R_T_C_Config_.rotate.acceleration.isRandom);
			if (b_S_R_T_C_Config_.rotate.acceleration.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.acc.rand.min", &b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("rotate.acc.rand.max", &b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x < b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x = b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x;
				}
				if (b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.y < b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.y)
				{
					b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.y = b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.y;
				}
				if (b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.z < b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.z)
				{
					b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.z = b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.z;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.rotate.acc", &b_S_R_T_C_Config_.rotate.acceleration.baseValue.x, 0.01f);
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomVal", &b_S_R_T_C_Config_.rotate.value.isRandom);
			if (b_S_R_T_C_Config_.rotate.value.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.rand.min", &b_S_R_T_C_Config_.rotate.value.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.rand.max", &b_S_R_T_C_Config_.rotate.value.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.value.randomRange_max.x < b_S_R_T_C_Config_.rotate.value.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.value.randomRange_max.x = b_S_R_T_C_Config_.rotate.value.randomRange_min.x;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("init.rotate.val", &b_S_R_T_C_Config_.rotate.value.baseValue.x, 0.01f);
			}

			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomVel", &b_S_R_T_C_Config_.rotate.velocity.isRandom);
			if (b_S_R_T_C_Config_.rotate.velocity.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.vel.rand.min", &b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.vel.rand.max", &b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x < b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.velocity.randomRange_max.x = b_S_R_T_C_Config_.rotate.velocity.randomRange_min.x;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("init.rotate.vel", &b_S_R_T_C_Config_.rotate.velocity.baseValue.x, 0.01f);
			}

			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &b_S_R_T_C_Config_.rotate.acceleration.isRandom);
			if (b_S_R_T_C_Config_.rotate.acceleration.isRandom)
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.acc.rand.min", &b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x, 0.01f);
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("rotate.acc.rand.max", &b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x, 0.01f);
				if (b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x < b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x)
				{
					b_S_R_T_C_Config_.rotate.acceleration.randomRange_max.x = b_S_R_T_C_Config_.rotate.acceleration.randomRange_min.x;
				}
			}
			else
			{
				requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat("init.rotate.acc", &b_S_R_T_C_Config_.rotate.acceleration.baseValue.x, 0.01f);
			}
		}
	}

	if (ImGui::CollapsingHeader("translate"))
	{
		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.translate.isRandomVal", &b_S_R_T_C_Config_.translate.value.isRandom);
		if (b_S_R_T_C_Config_.translate.value.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.rand.min", &b_S_R_T_C_Config_.translate.value.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.rand.max", &b_S_R_T_C_Config_.translate.value.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.translate.value.randomRange_max.x < b_S_R_T_C_Config_.translate.value.randomRange_min.x)
			{
				b_S_R_T_C_Config_.translate.value.randomRange_max.x = b_S_R_T_C_Config_.translate.value.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.translate.value.randomRange_max.y < b_S_R_T_C_Config_.translate.value.randomRange_min.y)
			{
				b_S_R_T_C_Config_.translate.value.randomRange_max.y = b_S_R_T_C_Config_.translate.value.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.translate.value.randomRange_max.z < b_S_R_T_C_Config_.translate.value.randomRange_min.z)
			{
				b_S_R_T_C_Config_.translate.value.randomRange_max.z = b_S_R_T_C_Config_.translate.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.translate.val", &b_S_R_T_C_Config_.translate.value.baseValue.x, 0.01f);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.translate.isRandomVel", &b_S_R_T_C_Config_.translate.velocity.isRandom);
		if (b_S_R_T_C_Config_.translate.velocity.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.vel.rand.min", &b_S_R_T_C_Config_.translate.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.vel.rand.max", &b_S_R_T_C_Config_.translate.velocity.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.translate.velocity.randomRange_max.x < b_S_R_T_C_Config_.translate.velocity.randomRange_min.x)
			{
				b_S_R_T_C_Config_.translate.velocity.randomRange_max.x = b_S_R_T_C_Config_.translate.velocity.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.translate.velocity.randomRange_max.y < b_S_R_T_C_Config_.translate.velocity.randomRange_min.y)
			{
				b_S_R_T_C_Config_.translate.velocity.randomRange_max.y = b_S_R_T_C_Config_.translate.velocity.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.translate.velocity.randomRange_max.z < b_S_R_T_C_Config_.translate.velocity.randomRange_min.z)
			{
				b_S_R_T_C_Config_.translate.velocity.randomRange_max.z = b_S_R_T_C_Config_.translate.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.translate.vel", &b_S_R_T_C_Config_.translate.velocity.baseValue.x, 0.01f);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.translate.isRandomAcc", &b_S_R_T_C_Config_.translate.acceleration.isRandom);
		if (b_S_R_T_C_Config_.translate.acceleration.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.acc.rand.min", &b_S_R_T_C_Config_.translate.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("translate.acc.rand.max", &b_S_R_T_C_Config_.translate.acceleration.randomRange_max.x, 0.01f);
			if (b_S_R_T_C_Config_.translate.acceleration.randomRange_max.x < b_S_R_T_C_Config_.translate.acceleration.randomRange_min.x)
			{
				b_S_R_T_C_Config_.translate.acceleration.randomRange_max.x = b_S_R_T_C_Config_.translate.acceleration.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.translate.acceleration.randomRange_max.y < b_S_R_T_C_Config_.translate.acceleration.randomRange_min.y)
			{
				b_S_R_T_C_Config_.translate.acceleration.randomRange_max.y = b_S_R_T_C_Config_.translate.acceleration.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.translate.acceleration.randomRange_max.z < b_S_R_T_C_Config_.translate.acceleration.randomRange_min.z)
			{
				b_S_R_T_C_Config_.translate.acceleration.randomRange_max.z = b_S_R_T_C_Config_.translate.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat3("init.translate.acc", &b_S_R_T_C_Config_.translate.acceleration.baseValue.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("color"))
	{
		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.color.isRandomVal", &b_S_R_T_C_Config_.color.value.isRandom);
		if (b_S_R_T_C_Config_.color.value.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::ColorEdit4("color.rand.min", &b_S_R_T_C_Config_.color.value.randomRange_min.x);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::ColorEdit4("color.rand.max", &b_S_R_T_C_Config_.color.value.randomRange_max.x);
			if (b_S_R_T_C_Config_.color.value.randomRange_max.x < b_S_R_T_C_Config_.color.value.randomRange_min.x)
			{
				b_S_R_T_C_Config_.color.value.randomRange_max.x = b_S_R_T_C_Config_.color.value.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.color.value.randomRange_max.y < b_S_R_T_C_Config_.color.value.randomRange_min.y)
			{
				b_S_R_T_C_Config_.color.value.randomRange_max.y = b_S_R_T_C_Config_.color.value.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.color.value.randomRange_max.z < b_S_R_T_C_Config_.color.value.randomRange_min.z)
			{
				b_S_R_T_C_Config_.color.value.randomRange_max.z = b_S_R_T_C_Config_.color.value.randomRange_min.z;
			}
			if (b_S_R_T_C_Config_.color.value.randomRange_max.w < b_S_R_T_C_Config_.color.value.randomRange_min.w)
			{
				b_S_R_T_C_Config_.color.value.randomRange_max.w = b_S_R_T_C_Config_.color.value.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::ColorEdit4("init.color.val", &b_S_R_T_C_Config_.color.value.baseValue.x);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.color.isRandomVel", &b_S_R_T_C_Config_.color.velocity.isRandom);
		if (b_S_R_T_C_Config_.color.velocity.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("color.vel.rand.min", &b_S_R_T_C_Config_.color.velocity.randomRange_min.x);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("color.vel.rand.max", &b_S_R_T_C_Config_.color.velocity.randomRange_max.x);
			if (b_S_R_T_C_Config_.color.velocity.randomRange_max.x < b_S_R_T_C_Config_.color.velocity.randomRange_min.x)
			{
				b_S_R_T_C_Config_.color.velocity.randomRange_max.x = b_S_R_T_C_Config_.color.velocity.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.color.velocity.randomRange_max.y < b_S_R_T_C_Config_.color.velocity.randomRange_min.y)
			{
				b_S_R_T_C_Config_.color.velocity.randomRange_max.y = b_S_R_T_C_Config_.color.velocity.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.color.velocity.randomRange_max.z < b_S_R_T_C_Config_.color.velocity.randomRange_min.z)
			{
				b_S_R_T_C_Config_.color.velocity.randomRange_max.z = b_S_R_T_C_Config_.color.velocity.randomRange_min.z;
			}
			if (b_S_R_T_C_Config_.color.velocity.randomRange_max.w < b_S_R_T_C_Config_.color.velocity.randomRange_min.w)
			{
				b_S_R_T_C_Config_.color.velocity.randomRange_max.w = b_S_R_T_C_Config_.color.velocity.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("init.color.vel", &b_S_R_T_C_Config_.color.velocity.baseValue.x);
		}

		requestRebuildEditingParticleByCurrentConfig_ |= ImGui::Checkbox("init.color.isRandomAcc", &b_S_R_T_C_Config_.color.acceleration.isRandom);
		if (b_S_R_T_C_Config_.color.acceleration.isRandom)
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("color.acc.rand.min", &b_S_R_T_C_Config_.color.acceleration.randomRange_min.x);
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("color.acc.rand.max", &b_S_R_T_C_Config_.color.acceleration.randomRange_max.x);
			if (b_S_R_T_C_Config_.color.acceleration.randomRange_max.x < b_S_R_T_C_Config_.color.acceleration.randomRange_min.x)
			{
				b_S_R_T_C_Config_.color.acceleration.randomRange_max.x = b_S_R_T_C_Config_.color.acceleration.randomRange_min.x;
			}
			if (b_S_R_T_C_Config_.color.acceleration.randomRange_max.y < b_S_R_T_C_Config_.color.acceleration.randomRange_min.y)
			{
				b_S_R_T_C_Config_.color.acceleration.randomRange_max.y = b_S_R_T_C_Config_.color.acceleration.randomRange_min.y;
			}
			if (b_S_R_T_C_Config_.color.acceleration.randomRange_max.z < b_S_R_T_C_Config_.color.acceleration.randomRange_min.z)
			{
				b_S_R_T_C_Config_.color.acceleration.randomRange_max.z = b_S_R_T_C_Config_.color.acceleration.randomRange_min.z;
			}
			if (b_S_R_T_C_Config_.color.acceleration.randomRange_max.w < b_S_R_T_C_Config_.color.acceleration.randomRange_min.w)
			{
				b_S_R_T_C_Config_.color.acceleration.randomRange_max.w = b_S_R_T_C_Config_.color.acceleration.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditingParticleByCurrentConfig_ |= ImGui::DragFloat4("init.color.acc", &b_S_R_T_C_Config_.color.acceleration.baseValue.x);
		}
	}

#endif
}


std::unique_ptr<IScene> PrticleEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();
	const Vector3 cameraPos = camera_->GetPosition();

	// ワールドグリッド更新
	grid_->Update(Vector3(0.0f, 0.0f, 0.0f), vp);

	// パーティクル再生成
	if (requestRebuildDrawingParticle_)
	{
		requestRebuildDrawingParticle_ = false;
		RebuildDrawParticle();
	}
	if (requestRebuildEditingParticleByJson_)
	{
		requestRebuildEditingParticleByJson_ = false;
		RebuildEditParticleByJson();
	}
	if (requestRebuildEditingParticleByCurrentConfig_)
	{
		requestRebuildEditingParticleByCurrentConfig_ = false;
		RebuildEditParticleByCurrentConfig();
	}

	// パーティクル更新
	Matrix4x4 worldMatrix = Matrix::MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.position);
	drawingParticle_.SetModelWorld(worldMatrix);
	drawingParticle_.SetCameraPos(cameraPos);
	drawingParticle_.Update(dt);
	editingParticle_.SetModelWorld(worldMatrix);
	editingParticle_.SetCameraPos(cameraPos);
	editingParticle_.Update(dt);

	// Zキーで切り替え
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z))
	{
		return std::make_unique<YokoScene>();
	}

	return nullptr;
}

void PrticleEditorScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);

	//if (isEmitterDraw_) emitterAABBRender_->Draw(cmdObj);

	drawingParticle_.Draw();
	editingParticle_.Draw();

	commonData_->particleDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
	DrawImGui();
#endif

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}