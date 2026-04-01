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

	void BuildParticle();

	void SaveData();
	void LoadData();

	void DrawImGui();

	void Reset(ParticleType type);

private:


private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// 共通Config
	Particle::Config particleConfig_{};
	// Fountain 固有
	FountainConfig fountainPreset_{};
	// OnTrailConfig 固有
	OnTrailConfig onTrailPreset_{};

	ParticleType currentType_ = ParticleType::Fountain;

	// Particle
	Particle particle_;
	bool requestRebuildParticle_ = false;
	Vector3 emitPos_{ 0.0f, 0.0f, 0.0f };

	// Json
	JsonManager json_;
	// DataBank
	ParticlePresetDataBank presetDataBank_;

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{ "particle_01" };
	char texturePathBuf_[256]{};
	char modelPathBuf_[256]{};
};