#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>
#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
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
		std::unique_ptr<SHEngine::RenderObject> render;
		std::string modelPath;
		int modelHandle = 0;
		int textureIndex = 0;
	};

private:
	void BuildModelList();
	void SelectModel(int index);
	void SetDefaultName();

	void CreateModelRender();
	void CreateMarkerRenders();
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
	std::vector<std::unique_ptr<DrawDataUnit>> modelRenders_;
	Transform modelTransform_{};

	// マーカー描画データ
	DrawDataUnit marker[2];


	TrailType currentType_ = TrailType::RibbonTrail;

	// 共通Config
	Trail::Config trailConfig_{};
	// Ribbon 固有
	RibbonTrailConfig ribbonPreset_{};
	// Shock 固有
	ShockwaveRingConfig shockPreset_{};

	// マーカー最終座標
	Vector3 markerPos[2];

	// Trail
	Trail trail_;
	bool emitTrail_ = true;
	bool requestRebuildTrail_ = false;

	// Json
	JsonManager json_;
	// DataBank
	TrailPresetDataBank presetDataBank_{};

	// ImGuiがstringを許容しないばかりに生まれてしまった産廃
	char presetNameBuf_[256]{ "Sword_Ribbon" };
	char texturePathBuf_[256]{};
};