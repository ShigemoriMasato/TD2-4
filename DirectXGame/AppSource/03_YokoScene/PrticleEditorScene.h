#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>
#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <memory>
#include <string>
#include <vector>
#include <Tool/Grid/Grid.h>

class PrticleEditorScene final : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	struct DrawDataUnit
	{
		std::string name;
		std::string modelPath;
		int modelIndex = -1;
		int textureIndex = 0;
	};

private:
	void BuildModelList();
	void BuildJsonList();

	void SelectModel(int index);

	void RebuildParticle();

	void SaveData();
	void LoadData();

	void DrawImGui();

	void UpdateRenders(const Matrix4x4& vpMatrix);


	void Reset(ParticleType type);

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	int selectedModelIndex_ = -1;

	// モデル描画データ
	std::vector<std::unique_ptr<DrawDataUnit>> modelDataList_;
	std::unique_ptr<SHEngine::RenderObject> modelRender_;
	Transform modelTransform_{};
	Matrix4x4 modelWorld_;
	bool isModelDraw_ = true;

	// エミッターAABB描画データ
	std::unique_ptr<SHEngine::RenderObject> emitterAABBRender_;
	bool isEmitterDraw_ = true;

	// 共通Config
	Particle::Config particleConfig_{};
	// Fountain 固有
	FountainConfig fountainPreset_{};
	// OnTrailConfig 固有
	OnTrailConfig onTrailPreset_{};
	// 上記Configを利用し描画するParticleが必要（編集中のParticleを描画するため）
	Particle editingParticle_;


	ParticleType currentType_ = ParticleType::Fountain;


	// Particle
	MultiParticle particle_;
	std::vector<std::string> activeParticleNameList_;
	bool requestRebuildParticle_ = false;

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{ "particle_01" };
	char texturePathBuf_[256]{};
	char modelPathBuf_[256]{};
	std::vector<std::string> JsonList_;
};