#include "PrticleEditorScene.h"
#include "03_YokoScene/YokoScene.h"

void PrticleEditorScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->Initialize(input_);

	Reset(ParticleType::Fountain);

	BuildParticle();
	LoadData();
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
	ChangeConfig_ = true;
}

void PrticleEditorScene::BuildParticle()
{
	particle_.Initialize(drawDataManager_, textureManager_, modelManager_, particleConfig_);

	particle_.Clear();

	particle_.Trigger(emitPos_);
}

// データ保存
void PrticleEditorScene::SaveData()
{
	if (presetNameBuf_[0] == '\0') return;

	const std::string fileBaseName = presetNameBuf_;
	json_.Boot(fileBaseName);

	// 共通：type
	{
		std::string type = ToString(currentType_);
		json_.Add("type", type);
	}

	// 共通：cfg
	{
		json_.Add("cfg.lifeTime", particleConfig_.lifeTime);
		json_.Add("cfg.speed", particleConfig_.speed);
		json_.Add("cfg.emitNum", particleConfig_.emitNum);
		json_.Add("cfg.emitInterval", particleConfig_.emitInterval);

		json_.Add("init.scale.isRandomVal", particleConfig_.scale.isRandom_value);
		json_.Add("init.scale.val", particleConfig_.scale.initial.value);
		json_.Add("init.scale.randomRangeValMin", particleConfig_.scale.randomRange_value_min);
		json_.Add("init.scale.randomRangeValMax", particleConfig_.scale.randomRange_value_max);
		json_.Add("init.scale.isRandomVel", particleConfig_.scale.isRandom_velocity);
		json_.Add("init.scale.vel", particleConfig_.scale.initial.velocity);
		json_.Add("init.scale.randomRangeVelMin", particleConfig_.scale.randomRange_velocity_min);
		json_.Add("init.scale.randomRangeVelMax", particleConfig_.scale.randomRange_velocity_max);
		json_.Add("init.scale.isRandomAcc", particleConfig_.scale.isRandom_acceleration);
		json_.Add("init.scale.acc", particleConfig_.scale.initial.acceleration);
		json_.Add("init.scale.randomRangeAccMin", particleConfig_.scale.randomRange_acceleration_min);
		json_.Add("init.scale.randomRangeAccMax", particleConfig_.scale.randomRange_acceleration_max);

		json_.Add("init.rotate.isRandomVal", particleConfig_.rotate.isRandom_value);
		json_.Add("init.rotate.val", particleConfig_.rotate.initial.value);
		json_.Add("init.rotate.randomRangeValMin", particleConfig_.rotate.randomRange_value_min);
		json_.Add("init.rotate.randomRangeValMax", particleConfig_.rotate.randomRange_value_max);
		json_.Add("init.rotate.isRandomVel", particleConfig_.rotate.isRandom_velocity);
		json_.Add("init.rotate.vel", particleConfig_.rotate.initial.velocity);
		json_.Add("init.rotate.randomRangeVelMin", particleConfig_.rotate.randomRange_velocity_min);
		json_.Add("init.rotate.randomRangeVelMax", particleConfig_.rotate.randomRange_velocity_max);
		json_.Add("init.rotate.isRandomAcc", particleConfig_.rotate.isRandom_acceleration);
		json_.Add("init.rotate.acc", particleConfig_.rotate.initial.acceleration);
		json_.Add("init.rotate.randomRangeAccMin", particleConfig_.rotate.randomRange_acceleration_min);
		json_.Add("init.rotate.randomRangeAccMax", particleConfig_.rotate.randomRange_acceleration_max);

		json_.Add("init.translate.isRandomVal", particleConfig_.translate.isRandom_value);
		json_.Add("init.translate.val", particleConfig_.translate.initial.value);
		json_.Add("init.translate.randomRangeValMin", particleConfig_.translate.randomRange_value_min);
		json_.Add("init.translate.randomRangeValMax", particleConfig_.translate.randomRange_value_max);
		json_.Add("init.translate.isRandomVel", particleConfig_.translate.isRandom_velocity);
		json_.Add("init.translate.vel", particleConfig_.translate.initial.velocity);
		json_.Add("init.translate.randomRangeVelMin", particleConfig_.translate.randomRange_velocity_min);
		json_.Add("init.translate.randomRangeVelMax", particleConfig_.translate.randomRange_velocity_max);
		json_.Add("init.translate.isRandomAcc", particleConfig_.translate.isRandom_acceleration);
		json_.Add("init.translate.acc", particleConfig_.translate.initial.acceleration);
		json_.Add("init.translate.randomRangeAccMin", particleConfig_.translate.randomRange_acceleration_min);
		json_.Add("init.translate.randomRangeAccMax", particleConfig_.translate.randomRange_acceleration_max);


		json_.Add("cfg.texturePath", particleConfig_.texturePath);
		json_.Add("cfg.modelPath", particleConfig_.modelPath);
	}

	// type固有
	if (currentType_ == ParticleType::Fountain)
	{
		// 追加項目が増えたらここへ
	}

	json_.Save();
}
// データ読み込み
void PrticleEditorScene::LoadData()
{
	if (presetNameBuf_[0] == '\0') return;

	const std::string fileBaseName = presetNameBuf_;
	json_.Boot(fileBaseName);

	// type
	{
		std::string typeStr;
		try { typeStr = json_.Get<std::string>("type"); }
		catch (...) { return; }

		ParticleType t{};
		if (!FromString(typeStr, t)) return;

		currentType_ = t;
	}

	// cfg.*
	try { particleConfig_.lifeTime = json_.Get<float>("cfg.lifeTime"); }
	catch (...) {}
	try { particleConfig_.speed = json_.Get<float>("cfg.speed"); }
	catch (...) {}
	try { particleConfig_.emitNum = json_.Get<int>("cfg.emitNum"); }
	catch (...) {}
	try { particleConfig_.emitInterval = json_.Get<float>("cfg.emitInterval"); }
	catch (...) {}

	try { particleConfig_.scale.isRandom_value = json_.Get<bool>("init.scale.isRandomVal"); }
	catch (...) {}
	try { particleConfig_.scale.initial.value = json_.Get<Vector3>("init.scale.val"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_value_min = json_.Get<Vector3>("init.scale.randomRangeValMin"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_value_max = json_.Get<Vector3>("init.scale.randomRangeValMax"); }
	catch (...) {}

	try { particleConfig_.scale.isRandom_velocity = json_.Get<bool>("init.scale.isRandomVel"); }
	catch (...) {}
	try { particleConfig_.scale.initial.velocity = json_.Get<Vector3>("init.scale.vel"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_velocity_min = json_.Get<Vector3>("init.scale.randomRangeVelMin"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_velocity_max = json_.Get<Vector3>("init.scale.randomRangeVelMax"); }
	catch (...) {}

	try { particleConfig_.scale.isRandom_acceleration = json_.Get<bool>("init.scale.isRandomAcc"); }
	catch (...) {}
	try { particleConfig_.scale.initial.acceleration = json_.Get<Vector3>("init.scale.acc"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_acceleration_min = json_.Get<Vector3>("init.scale.randomRangeAccMin"); }
	catch (...) {}
	try { particleConfig_.scale.randomRange_acceleration_max = json_.Get<Vector3>("init.scale.randomRangeAccMax"); }
	catch (...) {}

	try { particleConfig_.rotate.isRandom_value = json_.Get<bool>("init.rotate.isRandomVal"); }
	catch (...) {}
	try { particleConfig_.rotate.initial.value = json_.Get<Vector3>("init.rotate.val"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_value_min = json_.Get<Vector3>("init.rotate.randomRangeValMin"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_value_max = json_.Get<Vector3>("init.rotate.randomRangeValMax"); }
	catch (...) {}
	try { particleConfig_.rotate.isRandom_velocity = json_.Get<bool>("init.rotate.isRandomVel"); }
	catch (...) {}
	try { particleConfig_.rotate.initial.velocity = json_.Get<Vector3>("init.rotate.vel"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_velocity_min = json_.Get<Vector3>("init.rotate.randomRangeVelMin"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_velocity_max = json_.Get<Vector3>("init.rotate.randomRangeVelMax"); }
	catch (...) {}
	try { particleConfig_.rotate.isRandom_acceleration = json_.Get<bool>("init.rotate.isRandomAcc"); }
	catch (...) {}
	try { particleConfig_.rotate.initial.acceleration = json_.Get<Vector3>("init.rotate.acc"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_acceleration_min = json_.Get<Vector3>("init.rotate.randomRangeAccMin"); }
	catch (...) {}
	try { particleConfig_.rotate.randomRange_acceleration_max = json_.Get<Vector3>("init.rotate.randomRangeAccMax"); }
	catch (...) {}

	try { particleConfig_.translate.isRandom_value = json_.Get<bool>("init.translate.isRandomVal"); }
	catch (...) {}
	try { particleConfig_.translate.initial.value = json_.Get<Vector3>("init.translate.val"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_value_min = json_.Get<Vector3>("init.translate.randomRangeValMin"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_value_max = json_.Get<Vector3>("init.translate.randomRangeValMax"); }
	catch (...) {}
	try { particleConfig_.translate.isRandom_velocity = json_.Get<bool>("init.translate.isRandomVel"); }
	catch (...) {}
	try { particleConfig_.translate.initial.velocity = json_.Get<Vector3>("init.translate.vel"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_velocity_min = json_.Get<Vector3>("init.translate.randomRangeVelMin"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_velocity_max = json_.Get<Vector3>("init.translate.randomRangeVelMax"); }
	catch (...) {}
	try { particleConfig_.translate.isRandom_acceleration = json_.Get<bool>("init.translate.isRandomAcc"); }
	catch (...) {}
	try { particleConfig_.translate.initial.acceleration = json_.Get<Vector3>("init.translate.acc"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_acceleration_min = json_.Get<Vector3>("init.translate.randomRangeAccMin"); }
	catch (...) {}
	try { particleConfig_.translate.randomRange_acceleration_max = json_.Get<Vector3>("init.translate.randomRangeAccMax"); }
	catch (...) {}




	try { particleConfig_.texturePath = json_.Get<std::string>("cfg.texturePath"); }
	catch (...) {}
	try { particleConfig_.modelPath = json_.Get<std::string>("cfg.modelPath"); }
	catch (...) {}

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);

	std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
	strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);

	// type固有
	if (currentType_ == ParticleType::Fountain)
	{
	}

	ChangeConfig_ = true;
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
		ChangeConfig_ |= ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
		ChangeConfig_ |= ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f);
		ChangeConfig_ |= ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000);
		ChangeConfig_ |= ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.01f, 10.0f);
	}

	if (ImGui::CollapsingHeader("scale"))
	{
		ChangeConfig_ |= ImGui::Checkbox("init.scale.isRandomVal", &particleConfig_.scale.isRandom_value);
		if (particleConfig_.scale.isRandom_value)
		{
			ChangeConfig_ |= ImGui::DragFloat3("scale.rand.min", &particleConfig_.scale.randomRange_value_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("scale.rand.max", &particleConfig_.scale.randomRange_value_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.scale.val", &particleConfig_.scale.initial.value.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.scale.isRandomVel", &particleConfig_.scale.isRandom_velocity);
		if (particleConfig_.scale.isRandom_velocity)
		{
			ChangeConfig_ |= ImGui::DragFloat3("scale.vel.rand.min", &particleConfig_.scale.randomRange_velocity_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("scale.vel.rand.max", &particleConfig_.scale.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.scale.vel", &particleConfig_.scale.initial.velocity.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.scale.isRandomAcc", &particleConfig_.scale.isRandom_acceleration);
		if (particleConfig_.scale.isRandom_acceleration)
		{
			ChangeConfig_ |= ImGui::DragFloat3("scale.acc.rand.min", &particleConfig_.scale.randomRange_acceleration_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("scale.acc.rand.max", &particleConfig_.scale.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.scale.acc", &particleConfig_.scale.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("rotate"))
	{
		ChangeConfig_ |= ImGui::Checkbox("init.rotate.isRandomVal", &particleConfig_.rotate.isRandom_value);
		if (particleConfig_.rotate.isRandom_value)
		{
			ChangeConfig_ |= ImGui::DragFloat3("rotate.rand.min", &particleConfig_.rotate.randomRange_value_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("rotate.rand.max", &particleConfig_.rotate.randomRange_value_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.rotate.val", &particleConfig_.rotate.initial.value.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.rotate.isRandomVel", &particleConfig_.rotate.isRandom_velocity);
		if (particleConfig_.rotate.isRandom_velocity)
		{
			ChangeConfig_ |= ImGui::DragFloat3("rotate.vel.rand.min", &particleConfig_.rotate.randomRange_velocity_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("rotate.vel.rand.max", &particleConfig_.rotate.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.rotate.vel", &particleConfig_.rotate.initial.velocity.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.rotate.isRandomAcc", &particleConfig_.rotate.isRandom_acceleration);
		if (particleConfig_.rotate.isRandom_acceleration)
		{
			ChangeConfig_ |= ImGui::DragFloat3("rotate.acc.rand.min", &particleConfig_.rotate.randomRange_acceleration_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("rotate.acc.rand.max", &particleConfig_.rotate.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.rotate.acc", &particleConfig_.rotate.initial.acceleration.x, 0.01f);
		}
	}

	if (ImGui::CollapsingHeader("translate"))
	{
		ChangeConfig_ |= ImGui::Checkbox("init.translate.isRandomVal", &particleConfig_.translate.isRandom_value);
		if (particleConfig_.translate.isRandom_value)
		{
			ChangeConfig_ |= ImGui::DragFloat3("translate.rand.min", &particleConfig_.translate.randomRange_value_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("translate.rand.max", &particleConfig_.translate.randomRange_value_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.translate.val", &particleConfig_.translate.initial.value.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.translate.isRandomVel", &particleConfig_.translate.isRandom_velocity);
		if (particleConfig_.translate.isRandom_velocity)
		{
			ChangeConfig_ |= ImGui::DragFloat3("translate.vel.rand.min", &particleConfig_.translate.randomRange_velocity_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("translate.vel.rand.max", &particleConfig_.translate.randomRange_velocity_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.translate.vel", &particleConfig_.translate.initial.velocity.x, 0.01f);
		}
		ChangeConfig_ |= ImGui::Checkbox("init.translate.isRandomAcc", &particleConfig_.translate.isRandom_acceleration);
		if (particleConfig_.translate.isRandom_acceleration)
		{
			ChangeConfig_ |= ImGui::DragFloat3("translate.acc.rand.min", &particleConfig_.translate.randomRange_acceleration_min.x, 0.01f);
			ChangeConfig_ |= ImGui::DragFloat3("translate.acc.rand.max", &particleConfig_.translate.randomRange_acceleration_max.x, 0.01f);
		}
		else
		{
			ChangeConfig_ |= ImGui::DragFloat3("init.translate.acc", &particleConfig_.translate.initial.acceleration.x, 0.01f);
		}
	}

	ImGui::Separator();
	if (ImGui::InputText("cfg.texturePath", texturePathBuf_, sizeof(texturePathBuf_)))
	{
		particleConfig_.texturePath = texturePathBuf_;
		ChangeConfig_ = true;
	}
	if (ImGui::InputText("cfg.modelPath", modelPathBuf_, sizeof(modelPathBuf_)))
	{
		particleConfig_.modelPath = modelPathBuf_;
		ChangeConfig_ = true;
	}

	ImGui::Separator();
	ImGui::DragFloat3("EmitPos", &emitPos_.x, 0.01f);

	if (ImGui::Button("Save")) SaveData();
	ImGui::SameLine();
	if (ImGui::Button("Load")) LoadData();

	ImGui::Text("File: Assets/Json/%s.json", presetNameBuf_);

	ImGui::End();
#endif
}


std::unique_ptr<IScene> PrticleEditorScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	if (ChangeConfig_)
	{
		ChangeConfig_ = false;
		particle_.SetConfig(particleConfig_);
	}

	particle_.Update(dt, vp);


	// Zキーで決定
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

	particle_.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
	DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}