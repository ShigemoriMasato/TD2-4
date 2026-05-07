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
	bool requestRebuildDrawingParticle_ = false;
	// 編集パーティクルのみ再生成(Jsonのプリセットから再生成)
	void RebuildEditParticleByJson();
	bool requestRebuildEditingParticleByJson_ = false;
	// 編集パーティクルのみ再生成(CurrentEditorConfigから再生成)
	void RebuildEditParticleByCurrentConfig();
	bool requestRebuildEditingParticleByCurrentConfig_ = false;

	void SaveData();
	void LoadData();

	void DrawImGui();
	void DrawImGui_Config();
	void DrawImGui_Config_GoToTarget();
	void DrawImGui_Config_B_S_R_T_C();

	void Reset();

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// 共通Config
	ParticleConfig particleConfig_{};
	// GoToTarget 固有
	GoToTargetConfig goToTargetConfig_{};
	// B_S_R_T_C 固有
	B_S_R_T_C_Config b_S_R_T_C_Config_{};

	ParticleType currentType_ = ParticleType::B_S_R_T_C;

	// 編集しているParticle。ImGuiで編集している内容を反映させるためのParticle(ここには一つしかAddされない)
	MultiParticle editingParticle_;
	// 描画しているParticle。複数のプリセットをAddして描画するためのParticle(activeParticleNameList_のプリセットをAddする)
	MultiParticle drawingParticle_;
	// 描画しているParticleのプリセット名リスト
	std::vector<std::string> activeParticleNameList_;

	Transform worldTransform_;

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{};
};