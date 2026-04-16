#include "PrticleEditorScene.h"
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
	Reset(ParticleType::Fountain);

	// "Assets/Model"以下のモデルをリストアップしてmodelDataList_作成
	BuildModelList();
	// "Assets/Json/Particle"以下のjsonをリストアップしてJsonList_作成
	BuildJsonList();

	// RenderObject作成
	modelRender_ = CreateDataRO();

	// モデルトランスフォーム初期化
	modelTransform_.position = { 0.0f, 0.0f, 0.0f };
	modelTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	modelTransform_.scale = { 1.0f, 1.0f, 1.0f };

	// 初期モデルを選択
	SelectModel(0);

}

// 編集データ初期化
void PrticleEditorScene::Reset(ParticleType type)
{
	currentType_ = type;

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);

	std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
	strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);

	particleConfig_ = Particle::Config{};

	// Fountain
	fountainPreset_ = FountainConfig{};
	fountainPreset_.cfg = particleConfig_;

	// GoToTarget
	goToTargetPreset_ = GoToTargetConfig{};
	goToTargetPreset_.cfg = particleConfig_;

	// OnTrail
	onTrailPreset_ = OnTrailConfig{};
	onTrailPreset_.cfg = particleConfig_;
}

// "Assets/Model/"以下のモデルをリストアップしてmodelDataList_作成。武器追従トレイルにしか使わない機能だから簡易的でよい
void PrticleEditorScene::BuildModelList()
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
// "Assets/Json/Particle"以下のjsonをリストアップしてJsonList_作成
void PrticleEditorScene::BuildJsonList()
{
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
}

// 選択モデルを変更
void PrticleEditorScene::SelectModel(int index)
{
	if (index < 0 || index >= int(modelDataList_.size())) return;
	selectedModelIndex_ = index;
	auto modelData = modelManager_->GetNodeModelData(modelDataList_[index]->modelIndex);
	const auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	modelRender_->SetDrawData(drawData);
	modelDataList_[index]->textureIndex = modelData.materials[modelData.materialIndex.front()].textureIndex;
}

// Particleの再構築
void PrticleEditorScene::RebuildDrawParticle()
{
	particle_.Clear();
	particle_.Initialize(textureManager_, modelManager_, commonData_);
	for (const auto& name : activeParticleNameList_)
	{
		particle_.Add(name);
	}
}
void PrticleEditorScene::RebuildEditParticle()
{
	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingParticle_.Add(presetNameBuf_);
	ParticlePresetVariant presetVar = editingParticle_.GetConfig(slot);
	if (std::holds_alternative<FountainConfig>(presetVar))
	{
		fountainPreset_ = std::get<FountainConfig>(presetVar);
	}
	else if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		goToTargetPreset_ = std::get<GoToTargetConfig>(presetVar);
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		onTrailPreset_ = std::get<OnTrailConfig>(presetVar);
	}
}
void PrticleEditorScene::RebuildEditParticleCurrent()
{
	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingParticle_.Add(presetNameBuf_);
	if (currentType_ == ParticleType::Fountain)
	{
		editingParticle_.SetConfig(slot, fountainPreset_);
	}
	else if (currentType_ == ParticleType::GoToTarget)
	{
		editingParticle_.SetConfig(slot, goToTargetPreset_);
	}
	else if (currentType_ == ParticleType::OnTrail)
	{
		editingParticle_.SetConfig(slot, onTrailPreset_);
	}
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == ParticleType::Fountain)
	{
		fountainPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, fountainPreset_);
	}
	else if (currentType_ == ParticleType::GoToTarget)
	{
		goToTargetPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, goToTargetPreset_);
	}
	else if (currentType_ == ParticleType::OnTrail)
	{
		onTrailPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, onTrailPreset_);
	}
}

// データ読み込み
void PrticleEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	ParticlePresetVariant var{};
	var = commonData_->particlePresetDataBank.Get(presetNameBuf_);

	if (std::holds_alternative<FountainConfig>(var))
	{
		currentType_ = ParticleType::Fountain;
		fountainPreset_ = std::get<FountainConfig>(var);
		particleConfig_ = fountainPreset_.cfg;
		std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
		strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);
		std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
		strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);
	}
	else if (std::holds_alternative<GoToTargetConfig>(var))
	{
		currentType_ = ParticleType::GoToTarget;
		goToTargetPreset_ = std::get<GoToTargetConfig>(var);
		particleConfig_ = goToTargetPreset_.cfg;
		std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
		strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);
		std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
		strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);
	}
	else if (std::holds_alternative<OnTrailConfig>(var))
	{
		currentType_ = ParticleType::OnTrail;
		onTrailPreset_ = std::get<OnTrailConfig>(var);
		particleConfig_ = onTrailPreset_.cfg;
		std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
		strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);
		std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
		strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);
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
		// ⇩BeginListBoxに変更し、smallButtonもつけ、[削除]を追加描画リストから外せるようにする
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
					requestRebuildDrawParticle_ = true;
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
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}
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
					strncpy_s(presetNameBuf_, sizeof(presetNameBuf_), JsonList_[i].c_str(), _TRUNCATE);
					// EditerのデータをJsonのデータに合わせたい
					requestRebuildEditParticle_ = true;
				}

				ImGui::SameLine();

				if (ImGui::SmallButton("描画"))
				{
					activeParticleNameList_.push_back(JsonList_[i]);
					// 描画リストに追加
					requestRebuildDrawParticle_ = true;
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
		int t = 0;
		const char* items[] = { "Fountain", "GoToTarget", "OnTrail" };
		if (ImGui::Combo("type", &t, items, 1))
		{
			if (currentType_ != ParticleType(t))
			{
				Reset(ParticleType(t));
				requestRebuildEditParticleCurrent_ = true;
			}
		}
	}

	ImGui::SeparatorText("共通Config");
	{
		// particleConfig_が変更されたとき編集パーティクルをparticleConfig_に合わせる必要がある
		requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
		requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f);
		requestRebuildEditParticleCurrent_ |= ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000);
		requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.01f, 10.0f);

		if (ImGui::InputText("cfg.texturePath", texturePathBuf_, sizeof(texturePathBuf_)))
		{
			particleConfig_.texturePath = texturePathBuf_;
			requestRebuildEditParticleCurrent_ = true;
		}
		if (ImGui::InputText("cfg.modelPath", modelPathBuf_, sizeof(modelPathBuf_)))
		{
			particleConfig_.modelPath = modelPathBuf_;
			requestRebuildEditParticleCurrent_ = true;
		}
	}

	ImGui::SeparatorText("固有Config");

	if (currentType_ == ParticleType::Fountain)
	{
		DrawImGui_Fountain();
	}
	else if (currentType_ == ParticleType::GoToTarget)
	{
		DrawImGui_GoToTarget();
	}
	else if (currentType_ == ParticleType::OnTrail)
	{
		DrawImGui_OnTrail();
	}

	ImGui::Separator();

	if (ImGui::Button("Save")) SaveData();
	ImGui::Checkbox("モデル描画", &isModelDraw_);
	ImGui::Checkbox("エミッターAABB描画", &isEmitterDraw_);

	ImGui::End();
#endif
}

void PrticleEditorScene::DrawImGui_Fountain()
{
#ifdef USE_IMGUI
	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVal", &fountainPreset_.scale.isRandom_value);
		if (fountainPreset_.scale.isRandom_value)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.min", &fountainPreset_.scale.randomRange_value_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.max", &fountainPreset_.scale.randomRange_value_max.x, 0.01f);

			if (fountainPreset_.scale.randomRange_value_max.x < fountainPreset_.scale.randomRange_value_min.x)
			{
				fountainPreset_.scale.randomRange_value_max.x = fountainPreset_.scale.randomRange_value_min.x;
			}
			if (fountainPreset_.scale.randomRange_value_max.y < fountainPreset_.scale.randomRange_value_min.y)
			{
				fountainPreset_.scale.randomRange_value_max.y = fountainPreset_.scale.randomRange_value_min.y;
			}
			if (fountainPreset_.scale.randomRange_value_max.z < fountainPreset_.scale.randomRange_value_min.z)
			{
				fountainPreset_.scale.randomRange_value_max.z = fountainPreset_.scale.randomRange_value_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.val", &fountainPreset_.scale.initial.value.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVel", &fountainPreset_.scale.isRandom_velocity);
		if (fountainPreset_.scale.isRandom_velocity)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.min", &fountainPreset_.scale.randomRange_velocity_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.max", &fountainPreset_.scale.randomRange_velocity_max.x, 0.01f);

			if (fountainPreset_.scale.randomRange_velocity_max.x < fountainPreset_.scale.randomRange_velocity_min.x)
			{
				fountainPreset_.scale.randomRange_velocity_max.x = fountainPreset_.scale.randomRange_velocity_min.x;
			}
			if (fountainPreset_.scale.randomRange_velocity_max.y < fountainPreset_.scale.randomRange_velocity_min.y)
			{
				fountainPreset_.scale.randomRange_velocity_max.y = fountainPreset_.scale.randomRange_velocity_min.y;
			}
			if (fountainPreset_.scale.randomRange_velocity_max.z < fountainPreset_.scale.randomRange_velocity_min.z)
			{
				fountainPreset_.scale.randomRange_velocity_max.z = fountainPreset_.scale.randomRange_velocity_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.vel", &fountainPreset_.scale.initial.velocity.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomAcc", &fountainPreset_.scale.isRandom_acceleration);
		if (fountainPreset_.scale.isRandom_acceleration)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.min", &fountainPreset_.scale.randomRange_acceleration_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.max", &fountainPreset_.scale.randomRange_acceleration_max.x, 0.01f);

			if (fountainPreset_.scale.randomRange_acceleration_max.x < fountainPreset_.scale.randomRange_acceleration_min.x)
			{
				fountainPreset_.scale.randomRange_acceleration_max.x = fountainPreset_.scale.randomRange_acceleration_min.x;
			}
			if (fountainPreset_.scale.randomRange_acceleration_max.y < fountainPreset_.scale.randomRange_acceleration_min.y)
			{
				fountainPreset_.scale.randomRange_acceleration_max.y = fountainPreset_.scale.randomRange_acceleration_min.y;
			}
			if (fountainPreset_.scale.randomRange_acceleration_max.z < fountainPreset_.scale.randomRange_acceleration_min.z)
			{
				fountainPreset_.scale.randomRange_acceleration_max.z = fountainPreset_.scale.randomRange_acceleration_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.acc", &fountainPreset_.scale.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("rotate"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomVal", &fountainPreset_.rotate.isRandom_value);
		if (fountainPreset_.rotate.isRandom_value)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.rand.min", &fountainPreset_.rotate.randomRange_value_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.rand.max", &fountainPreset_.rotate.randomRange_value_max.x, 0.01f);

			if (fountainPreset_.rotate.randomRange_value_max.x < fountainPreset_.rotate.randomRange_value_min.x)
			{
				fountainPreset_.rotate.randomRange_value_max.x = fountainPreset_.rotate.randomRange_value_min.x;
			}
			if (fountainPreset_.rotate.randomRange_value_max.y < fountainPreset_.rotate.randomRange_value_min.y)
			{
				fountainPreset_.rotate.randomRange_value_max.y = fountainPreset_.rotate.randomRange_value_min.y;
			}
			if (fountainPreset_.rotate.randomRange_value_max.z < fountainPreset_.rotate.randomRange_value_min.z)
			{
				fountainPreset_.rotate.randomRange_value_max.z = fountainPreset_.rotate.randomRange_value_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.val", &fountainPreset_.rotate.initial.value.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomVel", &fountainPreset_.rotate.isRandom_velocity);
		if (fountainPreset_.rotate.isRandom_velocity)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.vel.rand.min", &fountainPreset_.rotate.randomRange_velocity_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.vel.rand.max", &fountainPreset_.rotate.randomRange_velocity_max.x, 0.01f);

			if (fountainPreset_.rotate.randomRange_velocity_max.x < fountainPreset_.rotate.randomRange_velocity_min.x)
			{
				fountainPreset_.rotate.randomRange_velocity_max.x = fountainPreset_.rotate.randomRange_velocity_min.x;
			}
			if (fountainPreset_.rotate.randomRange_velocity_max.y < fountainPreset_.rotate.randomRange_velocity_min.y)
			{
				fountainPreset_.rotate.randomRange_velocity_max.y = fountainPreset_.rotate.randomRange_velocity_min.y;
			}
			if (fountainPreset_.rotate.randomRange_velocity_max.z < fountainPreset_.rotate.randomRange_velocity_min.z)
			{
				fountainPreset_.rotate.randomRange_velocity_max.z = fountainPreset_.rotate.randomRange_velocity_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.vel", &fountainPreset_.rotate.initial.velocity.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &fountainPreset_.rotate.isRandom_acceleration);
		if (fountainPreset_.rotate.isRandom_acceleration)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.acc.rand.min", &fountainPreset_.rotate.randomRange_acceleration_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.acc.rand.max", &fountainPreset_.rotate.randomRange_acceleration_max.x, 0.01f);

			if (fountainPreset_.rotate.randomRange_acceleration_max.x < fountainPreset_.rotate.randomRange_acceleration_min.x)
			{
				fountainPreset_.rotate.randomRange_acceleration_max.x = fountainPreset_.rotate.randomRange_acceleration_min.x;
			}
			if (fountainPreset_.rotate.randomRange_acceleration_max.y < fountainPreset_.rotate.randomRange_acceleration_min.y)
			{
				fountainPreset_.rotate.randomRange_acceleration_max.y = fountainPreset_.rotate.randomRange_acceleration_min.y;
			}
			if (fountainPreset_.rotate.randomRange_acceleration_max.z < fountainPreset_.rotate.randomRange_acceleration_min.z)
			{
				fountainPreset_.rotate.randomRange_acceleration_max.z = fountainPreset_.rotate.randomRange_acceleration_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.acc", &fountainPreset_.rotate.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("translate"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomVal", &fountainPreset_.translate.isRandom_value);
		if (fountainPreset_.translate.isRandom_value)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.rand.min", &fountainPreset_.translate.randomRange_value_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.rand.max", &fountainPreset_.translate.randomRange_value_max.x, 0.01f);

			if (fountainPreset_.translate.randomRange_acceleration_max.x < fountainPreset_.translate.randomRange_acceleration_min.x)
			{
				fountainPreset_.translate.randomRange_acceleration_max.x = fountainPreset_.translate.randomRange_acceleration_min.x;
			}
			if (fountainPreset_.translate.randomRange_acceleration_max.y < fountainPreset_.translate.randomRange_acceleration_min.y)
			{
				fountainPreset_.translate.randomRange_acceleration_max.y = fountainPreset_.translate.randomRange_acceleration_min.y;
			}
			if (fountainPreset_.translate.randomRange_acceleration_max.z < fountainPreset_.translate.randomRange_acceleration_min.z)
			{
				fountainPreset_.translate.randomRange_acceleration_max.z = fountainPreset_.translate.randomRange_acceleration_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.val", &fountainPreset_.translate.initial.value.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomVel", &fountainPreset_.translate.isRandom_velocity);
		if (fountainPreset_.translate.isRandom_velocity)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.vel.rand.min", &fountainPreset_.translate.randomRange_velocity_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.vel.rand.max", &fountainPreset_.translate.randomRange_velocity_max.x, 0.01f);

			if (fountainPreset_.translate.randomRange_velocity_max.x < fountainPreset_.translate.randomRange_velocity_min.x)
			{
				fountainPreset_.translate.randomRange_velocity_max.x = fountainPreset_.translate.randomRange_velocity_min.x;
			}
			if (fountainPreset_.translate.randomRange_velocity_max.y < fountainPreset_.translate.randomRange_velocity_min.y)
			{
				fountainPreset_.translate.randomRange_velocity_max.y = fountainPreset_.translate.randomRange_velocity_min.y;
			}
			if (fountainPreset_.translate.randomRange_velocity_max.z < fountainPreset_.translate.randomRange_velocity_min.z)
			{
				fountainPreset_.translate.randomRange_velocity_max.z = fountainPreset_.translate.randomRange_velocity_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.vel", &fountainPreset_.translate.initial.velocity.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomAcc", &fountainPreset_.translate.isRandom_acceleration);
		if (fountainPreset_.translate.isRandom_acceleration)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.acc.rand.min", &fountainPreset_.translate.randomRange_acceleration_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.acc.rand.max", &fountainPreset_.translate.randomRange_acceleration_max.x, 0.01f);

			if (fountainPreset_.translate.randomRange_acceleration_max.x < fountainPreset_.translate.randomRange_acceleration_min.x)
			{
				fountainPreset_.translate.randomRange_acceleration_max.x = fountainPreset_.translate.randomRange_acceleration_min.x;
			}
			if (fountainPreset_.translate.randomRange_acceleration_max.y < fountainPreset_.translate.randomRange_acceleration_min.y)
			{
				fountainPreset_.translate.randomRange_acceleration_max.y = fountainPreset_.translate.randomRange_acceleration_min.y;
			}
			if (fountainPreset_.translate.randomRange_acceleration_max.z < fountainPreset_.translate.randomRange_acceleration_min.z)
			{
				fountainPreset_.translate.randomRange_acceleration_max.z = fountainPreset_.translate.randomRange_acceleration_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.acc", &fountainPreset_.translate.initial.acceleration.x, 0.01f);
		}
	}

#endif
}

void PrticleEditorScene::DrawImGui_GoToTarget()
{}

void PrticleEditorScene::DrawImGui_OnTrail()
{}


void PrticleEditorScene::UpdateRenders(const Matrix4x4& vpMatrix)
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
}

std::unique_ptr<IScene> PrticleEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	// ワールドグリッド更新
	grid_->Update(Vector3(0.0f, 0.0f, 0.0f), vp);

	// パーティクル再生成
	if (requestRebuildDrawParticle_)
	{
		requestRebuildDrawParticle_ = false;
		RebuildDrawParticle();
	}
	if (requestRebuildEditParticle_)
	{
		requestRebuildEditParticle_ = false;
		RebuildEditParticle();
	}
	if (requestRebuildEditParticleCurrent_)
	{
		requestRebuildEditParticleCurrent_ = false;
		RebuildEditParticleCurrent();
	}

	// モデル・エミッター更新
	UpdateRenders(vp);

	// パーティクル更新
	particle_.SetModelWorld(modelWorld_);
	particle_.Update(dt);
	editingParticle_.SetModelWorld(modelWorld_);
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

	//if (isModelDraw_)modelRender_->Draw(cmdObj);

	// if (isEmitterDraw_) emitterAABBRender_->Draw(cmdObj);

	particle_.Draw();
	editingParticle_.Draw();

	commonData_->particleDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	display->ToPresent(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
	DrawImGui();
#endif

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}