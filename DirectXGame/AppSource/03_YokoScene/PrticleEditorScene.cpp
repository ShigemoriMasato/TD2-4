#include "PrticleEditorScene.h"

void PrticleEditorScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 3.0f, -10.0f });
	camera_->Initialize(input_);

	Reset(ParticleType::Fountain);

	RebuildParticle();
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
	requestRebuildParticle_ = true;
}

void PrticleEditorScene::RebuildParticle()
{}

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

		json_.Add("cfg.texturePath", particleConfig_.texturePath);
		json_.Add("cfg.modelPath", particleConfig_.modelPath);

		json_.Add("cfg.emitterMin", particleConfig_.emitterMin);
		json_.Add("cfg.emitterMax", particleConfig_.emitterMax);
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
	try { particleConfig_.texturePath = json_.Get<std::string>("cfg.texturePath"); }
	catch (...) {}
	try { particleConfig_.modelPath = json_.Get<std::string>("cfg.modelPath"); }
	catch (...) {}
	try { particleConfig_.emitterMin = json_.Get<Vector3>("cfg.emitterMin"); }
	catch (...) {}
	try { particleConfig_.emitterMax = json_.Get<Vector3>("cfg.emitterMax"); }
	catch (...) {}

	std::memset(texturePathBuf_, 0, sizeof(texturePathBuf_));
	strncpy_s(texturePathBuf_, sizeof(texturePathBuf_), particleConfig_.texturePath.c_str(), _TRUNCATE);

	std::memset(modelPathBuf_, 0, sizeof(modelPathBuf_));
	strncpy_s(modelPathBuf_, sizeof(modelPathBuf_), particleConfig_.modelPath.c_str(), _TRUNCATE);

	// type固有
	if (currentType_ == ParticleType::Fountain)
	{
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

	requestRebuildParticle_ |= ImGui::DragFloat("cfg.lifeTime", &particleConfig_.lifeTime, 0.01f, 0.001f, 10.0f);
	requestRebuildParticle_ |= ImGui::DragFloat("cfg.speed", &particleConfig_.speed, 0.01f, 0.0f, 100.0f);
	requestRebuildParticle_ |= ImGui::DragInt("cfg.emitNum", &particleConfig_.emitNum, 1.0f, 1, 10000);
	requestRebuildParticle_ |= ImGui::DragFloat("cfg.emitInterval", &particleConfig_.emitInterval, 0.01f, 0.0f, 10.0f);

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

	requestRebuildParticle_ |= ImGui::DragFloat3("cfg.emitterMin", &particleConfig_.emitterMin.x, 0.01f);
	requestRebuildParticle_ |= ImGui::DragFloat3("cfg.emitterMax", &particleConfig_.emitterMax.x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat3("EmitPos", &emitPos_.x, 0.01f);
	ImGui::Checkbox("Emit", &emit_);

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

	if (requestRebuildParticle_)
	{
		requestRebuildParticle_ = false;
		RebuildParticle();
	}

	if (emit_)
	{
	}

	particle_.Update(dt);

	return nullptr;
}

void PrticleEditorScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	const Matrix4x4 vp = camera_->GetVPMatrix();
	particle_.Draw(cmdObj, vp);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();
	DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}