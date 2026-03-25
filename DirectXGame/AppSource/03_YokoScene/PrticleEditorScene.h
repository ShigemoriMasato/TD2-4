#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>
#include <memory>
#include <string>
#include <vector>

class PrticleEditorScene final : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void RebuildParticle();

	void SaveData();
	void LoadData();

	void DrawImGui();

	void Reset(ParticleType type);

private:
	std::unique_ptr<DebugCamera> camera_;

	ParticleType currentType_ = ParticleType::Fountain;

	// 共通Config
	Particle::Config particleConfig_{};
	// Fountain 固有
	FountainConfig fountainPreset_{};


	// Particle
	Particle particle_;
	bool emit_ = true;
	bool requestRebuildParticle_ = false;

	Vector3 emitPos_{ 0.0f, 0.0f, 0.0f };

	// Json
	JsonManager json_;

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{ "Fountain_01" };
	char texturePathBuf_[256]{};
	char modelPathBuf_[256]{};
};