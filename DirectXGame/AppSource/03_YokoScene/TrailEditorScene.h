#pragma once
#include <Scene/IScene.h>
#include <Tool/Grid/Grid.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>
#include <memory>
#include <string>
#include <vector>

class TrailEditorScene final : public IScene
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
	std::vector<std::string> jsonList_;

	void SelectModel(int index);


	// 描画トレイルのみ再生成
	void RebuildDrawTrail();
	bool requestRebuildDrawingTrail_ = false;
	// 編集トレイルのみ再生成(Jsonのプリセットから再生成)
	void RebuildEditTrailByJson();
	bool requestRebuildEditingTrailByJson_ = false;
	// 編集トレイルのみ再生成(CurrentEditorConfigから再生成)
	void RebuildEditTrailByCurrentConfig();
	bool requestRebuildEditingTrailByCurrentConfig_ = false;

	void RebuildTrail();

	void SaveData();
	void LoadData();

	void DrawImGui();
	void DrawImGui_Config();
	void DrawImGui_Config_Ribbon();
	void DrawImGui_Config_Shockwave();

	void UpdateRenders(const Matrix4x4& vpMatrix);

	void Reset();

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// 共通Config


	int selectedModelIndex_ = -1;

	// モデル描画データ
	std::vector<std::unique_ptr<DrawDataUnit>> modelDataList_;
	std::unique_ptr<SHEngine::RenderObject> modelRender_;
	Transform modelTransform_{};
	Matrix4x4 modelWorld_;
	bool isModelDraw_ = true;

	// マーカー描画データ
	std::unique_ptr<SHEngine::RenderObject> marker[2];
	Vector3 markerPos[2];
	bool isMarkerDraw_ = true;


	// 共通Config
	TrailConfig trailConfig_{};
	// Ribbon 固有
	RibbonTrailConfig ribbonPreset_{};
	// Shock 固有
	ShockwaveRingConfig shockPreset_{};
	// 上記Configを利用し描画するTrailが必要（編集中のトレイルを描画するため）
	Trail editingTrail_;


	TrailType currentType_ = TrailType::RibbonTrail;

	// Trail
	MultiTrail trail_;
	std::vector<std::string> activeTrailNameList_;
	bool requestRebuildTrail_ = false;

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{ "trail_01" };
	char texturePathBuf_[256]{};
};