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
	void BuildJsonList();

	void SelectModel(int index);

	// 描画パーティクルのみ再生成
	void RebuildDrawParticle();
	// 編集パーティクルのみ再生成(Jsonのプリセットから再生成)
	void RebuildEditParticle();
	// 編集パーティクルを現在の編集内容で再生成
	void RebuildEditParticleCurrent();

	void SaveData();
	void LoadData();

	void DrawImGui();
	void DrawImGui_Fountain();
	void DrawImGui_GoToTarget();
	void DrawImGui_OnTrail();

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
	// GoToTarget 固有
	GoToTargetConfig goToTargetPreset_{};
	// OnTrailConfig 固有
	OnTrailConfig onTrailPreset_{};
	// 上記Configを利用し描画するParticleが必要（編集中のParticleを描画するため）
	MultiParticle editingParticle_;


	ParticleType currentType_ = ParticleType::Fountain;


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
	std::vector<std::string> JsonList_;
};