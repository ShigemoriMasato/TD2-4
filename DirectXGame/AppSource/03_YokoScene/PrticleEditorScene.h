#pragma once
#include <Scene/IScene.h>
#include <Tool/Grid/Grid.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
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
	struct DrawDataUnit
	{
		std::string name;
		std::string modelPath;
		int modelIndex = -1;
		int textureIndex = 0;
	};

private:
	void BuildModelList();
	std::vector<std::string> modelList_;
	void BuildTextureList();
	std::vector<std::string> textureList_;
	void BuildJsonList();
	std::vector<std::string> JsonList_;

	//void SelectModel(int index);

	// 描画パーティクルのみ再生成
	void RebuildDrawParticle();
	// 編集パーティクルのみ再生成(Jsonのプリセットから再生成)
	void RebuildEditParticleByJson();
	// 編集パーティクルを現在の編集内容で再生成
	void RebuildEditParticleByCurrentConfig();

	void SaveData();
	void LoadData();

	void DrawImGui();
	void DrawImGui_Physics();
	void DrawImGui_GoToTarget();
	void DrawImGui_OnTrail();
	void DrawImGui_BillboardScale();
	void DrawImGui_BillboardScale2();

	void Reset(ParticleType type);

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// エミッターAABB描画データ
	std::unique_ptr<SHEngine::RenderObject> emitterAABBRender_;
	bool isEmitterDraw_ = true;

	// 共通Config
	Particle::Config particleConfig_{};
	// Physics 固有
	PhysicsConfig physicsPreset_{};
	// GoToTarget 固有
	GoToTargetConfig goToTargetPreset_{};
	// OnTrailConfig 固有
	OnTrailConfig onTrailPreset_{};
	// Billboard_Scale 固有
	BillboardScaleConfig billboardScalePreset_{};
	// Billboard_Scale2 固有
	BillboardScale2Config billboardScale2Preset_{};


	// 上記Configを利用し描画するParticleが必要（編集中のParticleを描画するため）(ここには一つしかAddされない)
	MultiParticle editingParticle_;


	ParticleType currentType_ = ParticleType::Physics;

	// Particle
	MultiParticle particle_;
	std::vector<std::string> activeParticleNameList_;
	// 描画パーティクルのみ再生成
	bool requestRebuildDrawParticle_ = false;
	// 編集パーティクルのみ再生成(Jsonのプリセットから再生成)
	bool requestRebuildEditParticle_ = false;
	// 編集パーティクルを現在のEditerConfigで再生成
	bool requestRebuildEditParticleCurrent_ = false;



	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{};
	char texturePathBuf_[256]{};
	char modelPathBuf_[256]{};
};