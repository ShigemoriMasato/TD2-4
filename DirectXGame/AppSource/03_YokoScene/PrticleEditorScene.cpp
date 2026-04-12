#include "PrticleEditorScene.h"
#include "03_YokoScene/YokoScene.h"

void PrticleEditorScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->Initialize(input_);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	// Drawer初期化（TrailEditorSceneのtrailDrawerと同じ）
	{
		ParticleDrawer::Config cfg{};
		commonData_->particleDrawer.Initialize(drawDataManager_, modelManager_, cfg);
	}

	Reset(ParticleType::Fountain);

	BuildParticle();
}

// 編集データ初期化
void PrticleEditorScene::Reset(ParticleType type)
{
	currentType_ = type;

	particleConfig_ = Particle::Config{};
	// Fountain
	fountainPreset_ = FountainConfig{};
	fountainPreset_.cfg = particleConfig_;

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);

	std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
	strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);

	emitPos_ = { 0.0f, 0.0f, 0.0f };
	requestRebuildParticle_ = true;
}

void PrticleEditorScene::BuildParticle()
{
	particle_.Initialize(drawDataManager_, textureManager_, modelManager_);
	particle_.SetConfig(particleConfig_);

	particle_.Clear();

	particle_.SetEmitPos(emitPos_);
	particle_.SetEmittingFlag(true);
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	presetDataBank_.Save(presetNameBuf_, currentType_, particleConfig_);
}

// データ読み込み
void PrticleEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	ParticlePresetVariant var{};
	var = presetDataBank_.Get(presetNameBuf_);

	if (std::holds_alternative<FountainConfig>(var))
	{
		currentType_ = ParticleType::Fountain;
		fountainPreset_ = std::get<FountainConfig>(var);
		particleConfig_ = fountainPreset_.cfg;
	}
	else if (std::holds_alternative<OnTrailConfig>(var))
	{
		currentType_ = ParticleType::OnTrail;
		onTrailPreset_ = std::get<OnTrailConfig>(var);
		particleConfig_ = onTrailPreset_.cfg;
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

std::unique_ptr<IScene> PrticleEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	grid_->Update(Vector3(0.0f, 0.0f, 0.0f), camera_->GetVPMatrix());
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	if (requestRebuildParticle_)
	{
		requestRebuildParticle_ = false;
		particle_.SetConfig(particleConfig_);
	}

	particle_.Update(dt, vp);

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

	// Drawer経由で描画（TrailEditorSceneのtrailDrawerと同じ運用）
	commonData_->particleDrawer.Clear();
	commonData_->particleDrawer.Register(&particle_);
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