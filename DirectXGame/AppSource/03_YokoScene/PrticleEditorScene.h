#pragma once
#include <Scene/IScene.h>
#include <Tool/Grid/Grid.h>
#include <Camera/DebugCamera.h>
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
	void DrawImGui_Config();
	void DrawImGui_Config_Physics();
	void DrawImGui_Config_GoToTarget();
	void DrawImGui_Config_OnTrail();
	void DrawImGui_Config_BillboardScale();
	void DrawImGui_Config_BillboardScale2();
	void DrawImGui_Config_BillboardColor();

	void Reset(ParticleType type);

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// 共通Config
	ParticleConfig particleConfig_{};
	// Physics 固有
	PhysicsConfig physicsConfig_{};
	// GoToTarget 固有
	GoToTargetConfig goToTargetConfig_{};
	// OnTrailConfig 固有
	OnTrailConfig onTrailConfig_{};
	// B_S 固有
	B_S_Config b_S_Config_{};
	// B_S_T 固有
	B_S_T_Config b_S_T_Config_{};
	// B_S_C 固有
	B_S_C_Config b_S_C_Config_{};
	// B_S_R_T_C 固有
	B_S_R_T_C_Config b_S_R_T_C_Config_{};

	// 上記Configを利用し描画するParticleが必要（編集中のParticleを描画するため）


	ParticleType currentType_ = ParticleType::Physics;

	// 編集しているParticle。ImGuiで編集している内容を反映させるためのParticle(ここには一つしかAddされない)
	MultiParticle editingParticle_;
	// 描画しているParticle。複数のプリセットをAddして描画するためのParticle(activeParticleNameList_のプリセットをAddする)
	MultiParticle drawingParticle_;
	// 描画しているParticleのプリセット名リスト
	std::vector<std::string> activeParticleNameList_;
	// 描画パーティクルのみ再生成
	bool requestRebuildDrawParticle_ = false;
	// 編集パーティクルのみ再生成(Jsonのプリセットから再生成)
	bool requestRebuildEditParticle_ = false;
	// 編集パーティクルを現在のEditerConfigで再生成
	bool requestRebuildEditParticleCurrent_ = false;


	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{};
};