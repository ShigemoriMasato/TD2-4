#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Trail/Trail.h>
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
	struct editDataUnit
	{
		std::string name;
		std::unique_ptr<SHEngine::RenderObject> render;
		int modelHandle = -1;
		int textureIndex = 0;
	};

private:
	void BuildModelList();
	void SelectModel(int index);

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

	void ApplyPresetToEditor_();
	void ResetEditorDefaults_(TrailPresetType type);

private:
	std::unique_ptr<DebugCamera> camera_;

	int selectedModelIndex_ = -1;
	std::vector<std::unique_ptr<editDataUnit>> modelRenders_;
	Transform modelTransform_{};
	Matrix4x4 modelWvp_{};

	// Marker
	std::unique_ptr<SHEngine::RenderObject> markerOriginRender_;
	std::unique_ptr<SHEngine::RenderObject> markerTipRender_;
	NodeModelData markerModelData_{};

	// === Editor State ===
	TrailPresetType currentType_ = TrailPresetType::RibbonTrail;

	// 共通Config（全部使わなくてもOK）
	Trail::Config trailConfig_{};

	// Ribbon 固有
	std::string ribbonModelName_;
	Vector3 originLocal_{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal_{ 0.0f, 0.55f, -3.2f };

	// Shock 固有
	ShockwaveRingPreset shockPreset_{};

	// 実行時計算
	Vector3 originWS_{};
	Vector3 tipWS_{};

	// Trail preview
	Trail trail_;
	bool emitTrail_ = true;
	bool requestRebuildTrail_ = false;

	// Json
	JsonManager json_;

	// Save name (拡張子なし)
	char presetNameBuf_[256]{ "Sword_Ribbon" };
	char texturePathBuf_[256]{};
};