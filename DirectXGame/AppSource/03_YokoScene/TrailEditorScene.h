#pragma once
#include <Scene/IScene.h>
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
		NodeModelData modelData;
		int textureIndex = 0;
	};

private:
	void BuildModelList();
	void BuildJsonList();

	void SelectModel(int index);

	void RebuildTrail();

	void SaveTrailData();
	void LoadTrailData();

	void DrawImGui();
	void UpdateRenders(const Matrix4x4& vpMatrix);

	// UIサポート
	void DrawConfigUI_();
	void DrawRibbonUI_();
	void DrawShockwaveUI_();

	void Reset(TrailType type);

private:

	std::unique_ptr<DebugCamera> camera_;

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
	Trail::Config trailConfig_{};
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
	std::vector<std::string> JsonList_;
};