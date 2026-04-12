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

	// ParticleDrawer初期化
	ParticleDrawer::Config cfg{};
	commonData_->particleDrawer.Initialize(drawDataManager_, modelManager_, cfg);

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

	RebuildParticle();
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
void PrticleEditorScene::RebuildParticle()
{
	particle_.Clear();
	particle_.Initialize(textureManager_, modelManager_, &commonData_->particlePresetDataBank);
	for (const auto& name : activeParticleNameList_)
	{
		particle_.Add(name);
	}
	particle_.RegisterToDrawer(&commonData_->particleDrawer);

	editingParticle_.Clear();
	editingParticle_.Initialize(textureManager_, modelManager_);
	editingParticle_.SetConfig(particleConfig_);
	commonData_->trailDrawer.Register(&editingParticle_);
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	if (currentType_ == ParticleType::Fountain)
	{
		fountainPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, currentType_, fountainPreset_.cfg);
	}
	else if (currentType_ == ParticleType::OnTrail)
	{
		onTrailPreset_.cfg = particleConfig_;
		commonData_->particlePresetDataBank.Save(presetNameBuf_, currentType_, onTrailPreset_.cfg);
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

	requestRebuildParticle_ = true;
}



void PrticleEditorScene::DrawImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("ParticleEditor");

	ImGui::InputText("PresetName (no ext)", presetNameBuf_, sizeof(presetNameBuf_));

	// type（現状Fountainのみ）
	{
		int t = 0;
		const char* items[] = { "Fountain" };
		ImGui::Combo("type", &t, items, 1);
		currentType_ = ParticleType::Fountain;
	}

	if (ImGui::CollapsingHeader("density"))
	{
		requestRebuildParticle_ |= ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
		requestRebuildParticle_ |= ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f);
		requestRebuildParticle_ |= ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000);
		requestRebuildParticle_ |= ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.01f, 10.0f);
	}

	if (ImGui::CollapsingHeader("scale"))
	{
		requestRebuildParticle_ |= ImGui::Checkbox("init.scale.isRandomVal", &particleConfig_.scale.isRandom_value);
		if (particleConfig_.scale.isRandom_value)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.rand.min", &particleConfig_.scale.randomRange_value_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.rand.max", &particleConfig_.scale.randomRange_value_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.scale.val", &particleConfig_.scale.initial.value.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.scale.isRandomVel", &particleConfig_.scale.isRandom_velocity);
		if (particleConfig_.scale.isRandom_velocity)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.vel.rand.min", &particleConfig_.scale.randomRange_velocity_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.vel.rand.max", &particleConfig_.scale.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.scale.vel", &particleConfig_.scale.initial.velocity.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.scale.isRandomAcc", &particleConfig_.scale.isRandom_acceleration);
		if (particleConfig_.scale.isRandom_acceleration)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.acc.rand.min", &particleConfig_.scale.randomRange_acceleration_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("scale.acc.rand.max", &particleConfig_.scale.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.scale.acc", &particleConfig_.scale.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("rotate"))
	{
		requestRebuildParticle_ |= ImGui::Checkbox("init.rotate.isRandomVal", &particleConfig_.rotate.isRandom_value);
		if (particleConfig_.rotate.isRandom_value)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.rand.min", &particleConfig_.rotate.randomRange_value_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.rand.max", &particleConfig_.rotate.randomRange_value_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.rotate.val", &particleConfig_.rotate.initial.value.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.rotate.isRandomVel", &particleConfig_.rotate.isRandom_velocity);
		if (particleConfig_.rotate.isRandom_velocity)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.vel.rand.min", &particleConfig_.rotate.randomRange_velocity_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.vel.rand.max", &particleConfig_.rotate.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.rotate.vel", &particleConfig_.rotate.initial.velocity.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &particleConfig_.rotate.isRandom_acceleration);
		if (particleConfig_.rotate.isRandom_acceleration)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.acc.rand.min", &particleConfig_.rotate.randomRange_acceleration_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("rotate.acc.rand.max", &particleConfig_.rotate.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.rotate.acc", &particleConfig_.rotate.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("translate"))
	{
		requestRebuildParticle_ |= ImGui::Checkbox("init.translate.isRandomVal", &particleConfig_.translate.isRandom_value);
		if (particleConfig_.translate.isRandom_value)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.rand.min", &particleConfig_.translate.randomRange_value_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.rand.max", &particleConfig_.translate.randomRange_value_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.translate.val", &particleConfig_.translate.initial.value.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.translate.isRandomVel", &particleConfig_.translate.isRandom_velocity);
		if (particleConfig_.translate.isRandom_velocity)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.vel.rand.min", &particleConfig_.translate.randomRange_velocity_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.vel.rand.max", &particleConfig_.translate.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.translate.vel", &particleConfig_.translate.initial.velocity.x, 0.01f);
		}
		requestRebuildParticle_ |= ImGui::Checkbox("init.translate.isRandomAcc", &particleConfig_.translate.isRandom_acceleration);
		if (particleConfig_.translate.isRandom_acceleration)
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.acc.rand.min", &particleConfig_.translate.randomRange_acceleration_min.x, 0.01f);
			requestRebuildParticle_ |= ImGui::DragFloat3("translate.acc.rand.max", &particleConfig_.translate.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			requestRebuildParticle_ |= ImGui::DragFloat3("init.translate.acc", &particleConfig_.translate.initial.acceleration.x, 0.01f);
		}
	}

	ImGui::Separator();
	if (ImGui::InputText("cfg.texturePath", texturePathBuf_, sizeof(texturePathBuf_)))
	{
		particleConfig_.texturePath = texturePathBuf_;
		requestRebuildParticle_ = true;
	}
	if (ImGui::InputText("cfg.modelPath", modelPathBuf_, sizeof(modelPathBuf_)))
	{
		particleConfig_.modelPath = modelPathBuf_;
		requestRebuildParticle_ = true;
	}

	ImGui::Separator();
	ImGui::DragFloat3("EmitPos", &emitPos_.x, 0.01f);

	if (ImGui::Button("Save")) SaveData();
	ImGui::SameLine();
	if (ImGui::Button("Load")) LoadData();

	ImGui::Text("File: Assets/Json/Particle/%s.json", presetNameBuf_);

	ImGui::End();
#endif
}


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
	if (requestRebuildParticle_)
	{
		requestRebuildParticle_ = false;
		RebuildParticle();
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

	if (isModelDraw_)modelRender_->Draw(cmdObj);

	// if (isEmitterDraw_) emitterAABBRender_->Draw(cmdObj);

	commonData_->particleDrawer.Draw(cmdObj, camera_->GetVPMatrix());

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
	DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}