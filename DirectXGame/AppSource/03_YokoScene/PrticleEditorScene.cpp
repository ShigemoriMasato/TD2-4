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
	Reset(ParticleType::Physics);

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
void PrticleEditorScene::Reset(ParticleType type)
{
	currentType_ = type;

	particleConfig_ = Particle::Config{};

	// Fountain
	physicsPreset_ = PhysicsConfig{};
	physicsPreset_.cfg = particleConfig_;

	// GoToTarget
	goToTargetPreset_ = GoToTargetConfig{};
	goToTargetPreset_.cfg = particleConfig_;

	// OnTrail
	onTrailPreset_ = OnTrailConfig{};
	onTrailPreset_.cfg = particleConfig_;

	// Billboard_Scale
	billboardScalePreset_ = BillboardScaleConfig{};
	billboardScalePreset_.cfg = particleConfig_;

	// Billboard_Scale2
	billboardScale2Preset_ = BillboardScale2Config{};
	billboardScale2Preset_.cfg = particleConfig_;
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
	particle_.Clear();
	particle_.Initialize(textureManager_, modelManager_, commonData_);
	for (const auto& name : activeParticleNameList_)
	{
		particle_.Add(name);
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
	if (std::holds_alternative<PhysicsConfig>(presetVar))
	{
		physicsPreset_ = std::get<PhysicsConfig>(presetVar);
		particleConfig_ = physicsPreset_.cfg;
		currentType_ = ParticleType::Physics;
	}
	else if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		goToTargetPreset_ = std::get<GoToTargetConfig>(presetVar);
		particleConfig_ = goToTargetPreset_.cfg;
		currentType_ = ParticleType::GoToTarget;
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		onTrailPreset_ = std::get<OnTrailConfig>(presetVar);
		particleConfig_ = onTrailPreset_.cfg;
		currentType_ = ParticleType::OnTrail;
	}
	else if (std::holds_alternative<BillboardScaleConfig>(presetVar))
	{
		billboardScalePreset_ = std::get<BillboardScaleConfig>(presetVar);
		particleConfig_ = billboardScalePreset_.cfg;
		currentType_ = ParticleType::Billboard_Scale;
	}
	else if (std::holds_alternative<BillboardScale2Config>(presetVar))
	{
		billboardScale2Preset_ = std::get<BillboardScale2Config>(presetVar);
		particleConfig_ = billboardScale2Preset_.cfg;
		currentType_ = ParticleType::Billboard_Scale2;
	}
	else if (std::holds_alternative<BillboardColorConfig>(presetVar))
	{
		billboardColorPreset_ = std::get<BillboardColorConfig>(presetVar);
		particleConfig_ = billboardColorPreset_.cfg;
		currentType_ = ParticleType::Billboard_Color;
	}
}
// 編集中のParticleの再構築。現在のConfigで再構築
void PrticleEditorScene::RebuildEditParticleByCurrentConfig()
{
	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_, commonData_);
	if (presetNameBuf_[0] == '\0') return;
	int32_t slot = editingParticle_.Add(presetNameBuf_);
	if (currentType_ == ParticleType::Physics)
	{
		physicsPreset_.cfg.modelPath = particleConfig_.modelPath;
		physicsPreset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, physicsPreset_);
	}
	else if (currentType_ == ParticleType::GoToTarget)
	{
		goToTargetPreset_.cfg.modelPath = particleConfig_.modelPath;
		goToTargetPreset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, goToTargetPreset_);
	}
	else if (currentType_ == ParticleType::OnTrail)
	{
		onTrailPreset_.cfg.modelPath = particleConfig_.modelPath;
		onTrailPreset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, onTrailPreset_);
	}
	else if (currentType_ == ParticleType::Billboard_Scale)
	{
		billboardScalePreset_.cfg.modelPath = particleConfig_.modelPath;
		billboardScalePreset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, billboardScalePreset_);
	}
	else if (currentType_ == ParticleType::Billboard_Scale2)
	{
		billboardScale2Preset_.cfg.modelPath = particleConfig_.modelPath;
		billboardScale2Preset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, billboardScale2Preset_);
	}
	else if (currentType_ == ParticleType::Billboard_Color)
	{
		billboardColorPreset_.cfg.modelPath = particleConfig_.modelPath;
		billboardColorPreset_.cfg.texturePath = particleConfig_.texturePath;
		editingParticle_.SetConfig(slot, billboardColorPreset_);
	}
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == ParticleType::Physics)
	{
		physicsPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, physicsPreset_);
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
	else if (currentType_ == ParticleType::Billboard_Scale)
	{
		billboardScalePreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, billboardScalePreset_);
	}
	else if (currentType_ == ParticleType::Billboard_Scale2)
	{
		billboardScale2Preset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, billboardScale2Preset_);
	}
	else if (currentType_ == ParticleType::Billboard_Color)
	{
		billboardColorPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, billboardColorPreset_);
	}
}

// データ読み込み
void PrticleEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	ParticlePresetVariant var{};
	var = commonData_->particlePresetDataBank.Get(presetNameBuf_);

	if (std::holds_alternative<PhysicsConfig>(var))
	{
		currentType_ = ParticleType::Physics;
		physicsPreset_ = std::get<PhysicsConfig>(var);
		particleConfig_ = physicsPreset_.cfg;
	}
	else if (std::holds_alternative<GoToTargetConfig>(var))
	{
		currentType_ = ParticleType::GoToTarget;
		goToTargetPreset_ = std::get<GoToTargetConfig>(var);
		particleConfig_ = goToTargetPreset_.cfg;
	}
	else if (std::holds_alternative<OnTrailConfig>(var))
	{
		currentType_ = ParticleType::OnTrail;
		onTrailPreset_ = std::get<OnTrailConfig>(var);
		particleConfig_ = onTrailPreset_.cfg;
	}
	else if (std::holds_alternative<BillboardScaleConfig>(var))
	{
		currentType_ = ParticleType::Billboard_Scale;
		billboardScalePreset_ = std::get<BillboardScaleConfig>(var);
		particleConfig_ = billboardScalePreset_.cfg;
	}
	else if (std::holds_alternative<BillboardScale2Config>(var))
	{
		currentType_ = ParticleType::Billboard_Scale2;
		billboardScale2Preset_ = std::get<BillboardScale2Config>(var);
		particleConfig_ = billboardScale2Preset_.cfg;
	}
	else if (std::holds_alternative<BillboardColorConfig>(var))
	{
		currentType_ = ParticleType::Billboard_Color;
		billboardColorPreset_ = std::get<BillboardColorConfig>(var);
		particleConfig_ = billboardColorPreset_.cfg;
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
		int t = int(currentType_);
		const char* items[] = { "Fountain", "GoToTarget", "OnTrail", "Billboard_Scale", "Billboard_Scale2", "Billboard_Color" };
		if (ImGui::Combo("type", &t, items, IM_ARRAYSIZE(items)))
		{
			Reset(ParticleType(t));
			requestRebuildEditParticleCurrent_ = true;
		}
	}

	ImGui::SeparatorText("共通Config");
	DrawImGui_Config();

	ImGui::SeparatorText("固有Config");
	switch (currentType_)
	{
	case ParticleType::Physics:
		DrawImGui_Config_Physics();
		break;
	case ParticleType::GoToTarget:
		DrawImGui_Config_GoToTarget();
		break;
	case ParticleType::OnTrail:
		DrawImGui_Config_OnTrail();
		break;
	case ParticleType::Billboard_Scale:
		DrawImGui_Config_BillboardScale();
		break;
	case ParticleType::Billboard_Scale2:
		DrawImGui_Config_BillboardScale2();
		break;
	case ParticleType::Billboard_Color:
		DrawImGui_Config_BillboardColor();
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
#endif
}

// DrawImGui_Config系でconfigが変更された場合はrequestRebuildEditParticleCurrent_フラグを立て、editingParticle_をconfifに合わせて再構築する。
void PrticleEditorScene::DrawImGui_Config()
{
#ifdef USE_IMGUI
	requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
	requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f);
	requestRebuildEditParticleCurrent_ |= ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000);
	requestRebuildEditParticleCurrent_ |= ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.01f, 10.0f);

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
					requestRebuildEditParticleCurrent_ = true;
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
					requestRebuildEditParticleCurrent_ = true;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}
#endif
}
void PrticleEditorScene::DrawImGui_Config_Physics()
{
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVal", &physicsPreset_.scale.value.isRandom);
		if (physicsPreset_.scale.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.min", &physicsPreset_.scale.value.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.max", &physicsPreset_.scale.value.randomRange_max.x, 0.01f);
			if (physicsPreset_.scale.value.randomRange_max.x < physicsPreset_.scale.value.randomRange_min.x)
			{
				physicsPreset_.scale.value.randomRange_max.x = physicsPreset_.scale.value.randomRange_min.x;
			}
			if (physicsPreset_.scale.value.randomRange_max.y < physicsPreset_.scale.value.randomRange_min.y)
			{
				physicsPreset_.scale.value.randomRange_max.y = physicsPreset_.scale.value.randomRange_min.y;
			}
			if (physicsPreset_.scale.value.randomRange_max.z < physicsPreset_.scale.value.randomRange_min.z)
			{
				physicsPreset_.scale.value.randomRange_max.z = physicsPreset_.scale.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.val", &physicsPreset_.scale.value.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVel", &physicsPreset_.scale.velocity.isRandom);
		if (physicsPreset_.scale.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.min", &physicsPreset_.scale.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.max", &physicsPreset_.scale.velocity.randomRange_max.x, 0.01f);
			if (physicsPreset_.scale.velocity.randomRange_max.x < physicsPreset_.scale.velocity.randomRange_min.x)
			{
				physicsPreset_.scale.velocity.randomRange_max.x = physicsPreset_.scale.velocity.randomRange_min.x;
			}
			if (physicsPreset_.scale.velocity.randomRange_max.y < physicsPreset_.scale.velocity.randomRange_min.y)
			{
				physicsPreset_.scale.velocity.randomRange_max.y = physicsPreset_.scale.velocity.randomRange_min.y;
			}
			if (physicsPreset_.scale.velocity.randomRange_max.z < physicsPreset_.scale.velocity.randomRange_min.z)
			{
				physicsPreset_.scale.velocity.randomRange_max.z = physicsPreset_.scale.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.vel", &physicsPreset_.scale.velocity.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomAcc", &physicsPreset_.scale.acceleration.isRandom);
		if (physicsPreset_.scale.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.min", &physicsPreset_.scale.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.max", &physicsPreset_.scale.acceleration.randomRange_max.x, 0.01f);

			if (physicsPreset_.scale.acceleration.randomRange_max.x < physicsPreset_.scale.acceleration.randomRange_min.x)
			{
				physicsPreset_.scale.acceleration.randomRange_max.x = physicsPreset_.scale.acceleration.randomRange_min.x;
			}
			if (physicsPreset_.scale.acceleration.randomRange_max.y < physicsPreset_.scale.acceleration.randomRange_min.y)
			{
				physicsPreset_.scale.acceleration.randomRange_max.y = physicsPreset_.scale.acceleration.randomRange_min.y;
			}
			if (physicsPreset_.scale.acceleration.randomRange_max.z < physicsPreset_.scale.acceleration.randomRange_min.z)
			{
				physicsPreset_.scale.acceleration.randomRange_max.z = physicsPreset_.scale.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.acc", &physicsPreset_.scale.acceleration.baseValue.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("rotate"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomVal", &physicsPreset_.rotate.value.isRandom);
		if (physicsPreset_.rotate.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.rand.min", &physicsPreset_.rotate.value.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.rand.max", &physicsPreset_.rotate.value.randomRange_max.x, 0.01f);
			if (physicsPreset_.rotate.value.randomRange_max.x < physicsPreset_.rotate.value.randomRange_min.x)
			{
				physicsPreset_.rotate.value.randomRange_max.x = physicsPreset_.rotate.value.randomRange_min.x;
			}
			if (physicsPreset_.rotate.value.randomRange_max.y < physicsPreset_.rotate.value.randomRange_min.y)
			{
				physicsPreset_.rotate.value.randomRange_max.y = physicsPreset_.rotate.value.randomRange_min.y;
			}
			if (physicsPreset_.rotate.value.randomRange_max.z < physicsPreset_.rotate.value.randomRange_min.z)
			{
				physicsPreset_.rotate.value.randomRange_max.z = physicsPreset_.rotate.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.val", &physicsPreset_.rotate.value.baseValue.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomVel", &physicsPreset_.rotate.velocity.isRandom);
		if (physicsPreset_.rotate.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.vel.rand.min", &physicsPreset_.rotate.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.vel.rand.max", &physicsPreset_.rotate.velocity.randomRange_max.x, 0.01f);

			if (physicsPreset_.rotate.velocity.randomRange_max.x < physicsPreset_.rotate.velocity.randomRange_min.x)
			{
				physicsPreset_.rotate.velocity.randomRange_max.x = physicsPreset_.rotate.velocity.randomRange_min.x;
			}
			if (physicsPreset_.rotate.velocity.randomRange_max.y < physicsPreset_.rotate.velocity.randomRange_min.y)
			{
				physicsPreset_.rotate.velocity.randomRange_max.y = physicsPreset_.rotate.velocity.randomRange_min.y;
			}
			if (physicsPreset_.rotate.velocity.randomRange_max.z < physicsPreset_.rotate.velocity.randomRange_min.z)
			{
				physicsPreset_.rotate.velocity.randomRange_max.z = physicsPreset_.rotate.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.vel", &physicsPreset_.rotate.velocity.baseValue.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &physicsPreset_.rotate.acceleration.isRandom);
		if (physicsPreset_.rotate.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.acc.rand.min", &physicsPreset_.rotate.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("rotate.acc.rand.max", &physicsPreset_.rotate.acceleration.randomRange_max.x, 0.01f);

			if (physicsPreset_.rotate.acceleration.randomRange_max.x < physicsPreset_.rotate.acceleration.randomRange_min.x)
			{
				physicsPreset_.rotate.acceleration.randomRange_max.x = physicsPreset_.rotate.acceleration.randomRange_min.x;
			}
			if (physicsPreset_.rotate.acceleration.randomRange_max.y < physicsPreset_.rotate.acceleration.randomRange_min.y)
			{
				physicsPreset_.rotate.acceleration.randomRange_max.y = physicsPreset_.rotate.acceleration.randomRange_min.y;
			}
			if (physicsPreset_.rotate.acceleration.randomRange_max.z < physicsPreset_.rotate.acceleration.randomRange_min.z)
			{
				physicsPreset_.rotate.acceleration.randomRange_max.z = physicsPreset_.rotate.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.rotate.acc", &physicsPreset_.rotate.acceleration.baseValue.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("translate"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomVal", &physicsPreset_.translate.value.isRandom);
		if (physicsPreset_.translate.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.rand.min", &physicsPreset_.translate.value.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.rand.max", &physicsPreset_.translate.value.randomRange_max.x, 0.01f);
			if (physicsPreset_.translate.value.randomRange_max.x < physicsPreset_.translate.value.randomRange_min.x)
			{
				physicsPreset_.translate.value.randomRange_max.x = physicsPreset_.translate.value.randomRange_min.x;
			}
			if (physicsPreset_.translate.value.randomRange_max.y < physicsPreset_.translate.value.randomRange_min.y)
			{
				physicsPreset_.translate.value.randomRange_max.y = physicsPreset_.translate.value.randomRange_min.y;
			}
			if (physicsPreset_.translate.value.randomRange_max.z < physicsPreset_.translate.value.randomRange_min.z)
			{
				physicsPreset_.translate.value.randomRange_max.z = physicsPreset_.translate.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.val", &physicsPreset_.translate.value.baseValue.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomVel", &physicsPreset_.translate.velocity.isRandom);
		if (physicsPreset_.translate.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.vel.rand.min", &physicsPreset_.translate.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.vel.rand.max", &physicsPreset_.translate.velocity.randomRange_max.x, 0.01f);

			if (physicsPreset_.translate.velocity.randomRange_max.x < physicsPreset_.translate.velocity.randomRange_min.x)
			{
				physicsPreset_.translate.velocity.randomRange_max.x = physicsPreset_.translate.velocity.randomRange_min.x;
			}
			if (physicsPreset_.translate.velocity.randomRange_max.y < physicsPreset_.translate.velocity.randomRange_min.y)
			{
				physicsPreset_.translate.velocity.randomRange_max.y = physicsPreset_.translate.velocity.randomRange_min.y;
			}
			if (physicsPreset_.translate.velocity.randomRange_max.z < physicsPreset_.translate.velocity.randomRange_min.z)
			{
				physicsPreset_.translate.velocity.randomRange_max.z = physicsPreset_.translate.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.vel", &physicsPreset_.translate.velocity.baseValue.x, 0.01f);
		}
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.translate.isRandomAcc", &physicsPreset_.translate.acceleration.isRandom);
		if (physicsPreset_.translate.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.acc.rand.min", &physicsPreset_.translate.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("translate.acc.rand.max", &physicsPreset_.translate.acceleration.randomRange_max.x, 0.01f);

			if (physicsPreset_.translate.acceleration.randomRange_max.x < physicsPreset_.translate.acceleration.randomRange_min.x)
			{
				physicsPreset_.translate.acceleration.randomRange_max.x = physicsPreset_.translate.acceleration.randomRange_min.x;
			}
			if (physicsPreset_.translate.acceleration.randomRange_max.y < physicsPreset_.translate.acceleration.randomRange_min.y)
			{
				physicsPreset_.translate.acceleration.randomRange_max.y = physicsPreset_.translate.acceleration.randomRange_min.y;
			}
			if (physicsPreset_.translate.acceleration.randomRange_max.z < physicsPreset_.translate.acceleration.randomRange_min.z)
			{
				physicsPreset_.translate.acceleration.randomRange_max.z = physicsPreset_.translate.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.translate.acc", &physicsPreset_.translate.acceleration.baseValue.x, 0.01f);
		}
	}

#endif
}
void PrticleEditorScene::DrawImGui_Config_GoToTarget()
{}
void PrticleEditorScene::DrawImGui_Config_OnTrail()
{}
void PrticleEditorScene::DrawImGui_Config_BillboardScale()
{
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVal", &billboardScalePreset_.scale.value.isRandom);
		if (billboardScalePreset_.scale.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.min", &billboardScalePreset_.scale.value.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.max", &billboardScalePreset_.scale.value.randomRange_max.x, 0.01f);
			if (billboardScalePreset_.scale.value.randomRange_max.x < billboardScalePreset_.scale.value.randomRange_min.x)
			{
				billboardScalePreset_.scale.value.randomRange_max.x = billboardScalePreset_.scale.value.randomRange_min.x;
			}
			if (billboardScalePreset_.scale.value.randomRange_max.y < billboardScalePreset_.scale.value.randomRange_min.y)
			{
				billboardScalePreset_.scale.value.randomRange_max.y = billboardScalePreset_.scale.value.randomRange_min.y;
			}
			if (billboardScalePreset_.scale.value.randomRange_max.z < billboardScalePreset_.scale.value.randomRange_min.z)
			{
				billboardScalePreset_.scale.value.randomRange_max.z = billboardScalePreset_.scale.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.val", &billboardScalePreset_.scale.value.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVel", &billboardScalePreset_.scale.velocity.isRandom);
		if (billboardScalePreset_.scale.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.min", &billboardScalePreset_.scale.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.max", &billboardScalePreset_.scale.velocity.randomRange_max.x, 0.01f);
			if (billboardScalePreset_.scale.velocity.randomRange_max.x < billboardScalePreset_.scale.velocity.randomRange_min.x)
			{
				billboardScalePreset_.scale.velocity.randomRange_max.x = billboardScalePreset_.scale.velocity.randomRange_min.x;
			}
			if (billboardScalePreset_.scale.velocity.randomRange_max.y < billboardScalePreset_.scale.velocity.randomRange_min.y)
			{
				billboardScalePreset_.scale.velocity.randomRange_max.y = billboardScalePreset_.scale.velocity.randomRange_min.y;
			}
			if (billboardScalePreset_.scale.velocity.randomRange_max.z < billboardScalePreset_.scale.velocity.randomRange_min.z)
			{
				billboardScalePreset_.scale.velocity.randomRange_max.z = billboardScalePreset_.scale.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.vel", &billboardScalePreset_.scale.velocity.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomAcc", &billboardScalePreset_.scale.acceleration.isRandom);
		if (billboardScalePreset_.scale.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.min", &billboardScalePreset_.scale.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.max", &billboardScalePreset_.scale.acceleration.randomRange_max.x, 0.01f);

			if (billboardScalePreset_.scale.acceleration.randomRange_max.x < billboardScalePreset_.scale.acceleration.randomRange_min.x)
			{
				billboardScalePreset_.scale.acceleration.randomRange_max.x = billboardScalePreset_.scale.acceleration.randomRange_min.x;
			}
			if (billboardScalePreset_.scale.acceleration.randomRange_max.y < billboardScalePreset_.scale.acceleration.randomRange_min.y)
			{
				billboardScalePreset_.scale.acceleration.randomRange_max.y = billboardScalePreset_.scale.acceleration.randomRange_min.y;
			}
			if (billboardScalePreset_.scale.acceleration.randomRange_max.z < billboardScalePreset_.scale.acceleration.randomRange_min.z)
			{
				billboardScalePreset_.scale.acceleration.randomRange_max.z = billboardScalePreset_.scale.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.acc", &billboardScalePreset_.scale.acceleration.baseValue.x, 0.01f);
		}
	}

#endif
}
void PrticleEditorScene::DrawImGui_Config_BillboardScale2()
{}
void PrticleEditorScene::DrawImGui_Config_BillboardColor()
{
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVal", &billboardColorPreset_.scale.value.isRandom);
		if (billboardColorPreset_.scale.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.min", &billboardColorPreset_.scale.value.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.rand.max", &billboardColorPreset_.scale.value.randomRange_max.x, 0.01f);
			if (billboardColorPreset_.scale.value.randomRange_max.x < billboardColorPreset_.scale.value.randomRange_min.x)
			{
				billboardColorPreset_.scale.value.randomRange_max.x = billboardColorPreset_.scale.value.randomRange_min.x;
			}
			if (billboardColorPreset_.scale.value.randomRange_max.y < billboardColorPreset_.scale.value.randomRange_min.y)
			{
				billboardColorPreset_.scale.value.randomRange_max.y = billboardColorPreset_.scale.value.randomRange_min.y;
			}
			if (billboardColorPreset_.scale.value.randomRange_max.z < billboardColorPreset_.scale.value.randomRange_min.z)
			{
				billboardColorPreset_.scale.value.randomRange_max.z = billboardColorPreset_.scale.value.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.val", &billboardColorPreset_.scale.value.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomVel", &billboardColorPreset_.scale.velocity.isRandom);
		if (billboardColorPreset_.scale.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.min", &billboardColorPreset_.scale.velocity.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.vel.rand.max", &billboardColorPreset_.scale.velocity.randomRange_max.x, 0.01f);
			if (billboardColorPreset_.scale.velocity.randomRange_max.x < billboardColorPreset_.scale.velocity.randomRange_min.x)
			{
				billboardColorPreset_.scale.velocity.randomRange_max.x = billboardColorPreset_.scale.velocity.randomRange_min.x;
			}
			if (billboardColorPreset_.scale.velocity.randomRange_max.y < billboardColorPreset_.scale.velocity.randomRange_min.y)
			{
				billboardColorPreset_.scale.velocity.randomRange_max.y = billboardColorPreset_.scale.velocity.randomRange_min.y;
			}
			if (billboardColorPreset_.scale.velocity.randomRange_max.z < billboardColorPreset_.scale.velocity.randomRange_min.z)
			{
				billboardColorPreset_.scale.velocity.randomRange_max.z = billboardColorPreset_.scale.velocity.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.vel", &billboardColorPreset_.scale.velocity.baseValue.x, 0.01f);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.scale.isRandomAcc", &billboardColorPreset_.scale.acceleration.isRandom);
		if (billboardColorPreset_.scale.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.min", &billboardColorPreset_.scale.acceleration.randomRange_min.x, 0.01f);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("scale.acc.rand.max", &billboardColorPreset_.scale.acceleration.randomRange_max.x, 0.01f);

			if (billboardColorPreset_.scale.acceleration.randomRange_max.x < billboardColorPreset_.scale.acceleration.randomRange_min.x)
			{
				billboardColorPreset_.scale.acceleration.randomRange_max.x = billboardColorPreset_.scale.acceleration.randomRange_min.x;
			}
			if (billboardColorPreset_.scale.acceleration.randomRange_max.y < billboardColorPreset_.scale.acceleration.randomRange_min.y)
			{
				billboardColorPreset_.scale.acceleration.randomRange_max.y = billboardColorPreset_.scale.acceleration.randomRange_min.y;
			}
			if (billboardColorPreset_.scale.acceleration.randomRange_max.z < billboardColorPreset_.scale.acceleration.randomRange_min.z)
			{
				billboardColorPreset_.scale.acceleration.randomRange_max.z = billboardColorPreset_.scale.acceleration.randomRange_min.z;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat3("init.scale.acc", &billboardColorPreset_.scale.acceleration.baseValue.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("color"))
	{
		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.color.isRandomVal", &billboardColorPreset_.color.value.isRandom);
		if (billboardColorPreset_.color.value.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::ColorEdit4("color.rand.min", &billboardColorPreset_.color.value.randomRange_min.x);
			requestRebuildEditParticleCurrent_ |= ImGui::ColorEdit4("color.rand.max", &billboardColorPreset_.color.value.randomRange_max.x);
			if (billboardColorPreset_.color.value.randomRange_max.x < billboardColorPreset_.color.value.randomRange_min.x)
			{
				billboardColorPreset_.color.value.randomRange_max.x = billboardColorPreset_.color.value.randomRange_min.x;
			}
			if (billboardColorPreset_.color.value.randomRange_max.y < billboardColorPreset_.color.value.randomRange_min.y)
			{
				billboardColorPreset_.color.value.randomRange_max.y = billboardColorPreset_.color.value.randomRange_min.y;
			}
			if (billboardColorPreset_.color.value.randomRange_max.z < billboardColorPreset_.color.value.randomRange_min.z)
			{
				billboardColorPreset_.color.value.randomRange_max.z = billboardColorPreset_.color.value.randomRange_min.z;
			}
			if (billboardColorPreset_.color.value.randomRange_max.w < billboardColorPreset_.color.value.randomRange_min.w)
			{
				billboardColorPreset_.color.value.randomRange_max.w = billboardColorPreset_.color.value.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::ColorEdit4("init.color.val", &billboardColorPreset_.color.value.baseValue.x);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.color.isRandomVel", &billboardColorPreset_.color.velocity.isRandom);
		if (billboardColorPreset_.color.velocity.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("color.vel.rand.min", &billboardColorPreset_.color.velocity.randomRange_min.x);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("color.vel.rand.max", &billboardColorPreset_.color.velocity.randomRange_max.x);
			if (billboardColorPreset_.color.velocity.randomRange_max.x < billboardColorPreset_.color.velocity.randomRange_min.x)
			{
				billboardColorPreset_.color.velocity.randomRange_max.x = billboardColorPreset_.color.velocity.randomRange_min.x;
			}
			if (billboardColorPreset_.color.velocity.randomRange_max.y < billboardColorPreset_.color.velocity.randomRange_min.y)
			{
				billboardColorPreset_.color.velocity.randomRange_max.y = billboardColorPreset_.color.velocity.randomRange_min.y;
			}
			if (billboardColorPreset_.color.velocity.randomRange_max.z < billboardColorPreset_.color.velocity.randomRange_min.z)
			{
				billboardColorPreset_.color.velocity.randomRange_max.z = billboardColorPreset_.color.velocity.randomRange_min.z;
			}
			if (billboardColorPreset_.color.velocity.randomRange_max.w < billboardColorPreset_.color.velocity.randomRange_min.w)
			{
				billboardColorPreset_.color.velocity.randomRange_max.w = billboardColorPreset_.color.velocity.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("init.color.vel", &billboardColorPreset_.color.velocity.baseValue.x);
		}

		requestRebuildEditParticleCurrent_ |= ImGui::Checkbox("init.color.isRandomAcc", &billboardColorPreset_.color.acceleration.isRandom);
		if (billboardColorPreset_.color.acceleration.isRandom)
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("color.acc.rand.min", &billboardColorPreset_.color.acceleration.randomRange_min.x);
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("color.acc.rand.max", &billboardColorPreset_.color.acceleration.randomRange_max.x);
			if (billboardColorPreset_.color.acceleration.randomRange_max.x < billboardColorPreset_.color.acceleration.randomRange_min.x)
			{
				billboardColorPreset_.color.acceleration.randomRange_max.x = billboardColorPreset_.color.acceleration.randomRange_min.x;
			}
			if (billboardColorPreset_.color.acceleration.randomRange_max.y < billboardColorPreset_.color.acceleration.randomRange_min.y)
			{
				billboardColorPreset_.color.acceleration.randomRange_max.y = billboardColorPreset_.color.acceleration.randomRange_min.y;
			}
			if (billboardColorPreset_.color.acceleration.randomRange_max.z < billboardColorPreset_.color.acceleration.randomRange_min.z)
			{
				billboardColorPreset_.color.acceleration.randomRange_max.z = billboardColorPreset_.color.acceleration.randomRange_min.z;
			}
			if (billboardColorPreset_.color.acceleration.randomRange_max.w < billboardColorPreset_.color.acceleration.randomRange_min.w)
			{
				billboardColorPreset_.color.acceleration.randomRange_max.w = billboardColorPreset_.color.acceleration.randomRange_min.w;
			}
		}
		else
		{
			requestRebuildEditParticleCurrent_ |= ImGui::DragFloat4("init.color.acc", &billboardColorPreset_.color.acceleration.baseValue.x);
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
		RebuildEditParticleByJson();
	}
	if (requestRebuildEditParticleCurrent_)
	{
		requestRebuildEditParticleCurrent_ = false;
		RebuildEditParticleByCurrentConfig();
	}

	// パーティクル更新
	const Vector3 cameraPos = camera_->GetPosition();
	particle_.SetModelWorld(Matrix4x4::Identity());
	particle_.SetCameraPos(cameraPos);
	particle_.Update(dt);
	editingParticle_.SetModelWorld(Matrix4x4::Identity());
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

	particle_.Draw();
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