#pragma once
#include <Scene/IScene.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Trail/Trail.h>
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
	struct ModelEntry
	{
		std::string name; // 末端フォルダ名（例: Sword）
		std::string path; // LoadModel用（例: Assets/Model/Item/Weapon/Sword）
		int modelHandle = -1;
		NodeModelData modelData{};
		int textureIndex = 0;
	};

private:
	// モデル一覧
	void BuildModelList();
	void SelectModel(int index);

	// 描画初期化
	void CreateModelRender();
	void CreateMarkerRenders();
	void RebuildTrail();

	// 保存/ロード
	void SaveTrailData();
	void LoadTrailData();

	// UI
	void DrawImGui();

	// 更新処理
	void UpdateRenders(const Matrix4x4& vpMatrix);

private:
	std::unique_ptr<DebugCamera> camera_;

	// 選択モデル一覧
	std::vector<ModelEntry> models_;
	int selectedModelIndex_ = -1;

	// モデル描画
	std::unique_ptr<SHEngine::RenderObject> modelRender_;
	Transform modelTransform_{};
	Matrix4x4 modelWvp_{};
	int modelTextureIndex_ = 0;

	// マーカー（Cube）
	std::unique_ptr<SHEngine::RenderObject> markerOriginRender_;
	std::unique_ptr<SHEngine::RenderObject> markerTipRender_;
	int markerModelHandle_ = -1;
	NodeModelData markerModelData_{};

	Vector3 originLocal_{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal_{ 0.0f, 0.55f, -3.2f };

	Vector3 originWS_{};
	Vector3 tipWS_{};

	float markerScale_ = 0.15f;
	Vector4 markerOriginColor_{ 1.0f, 0.2f, 0.2f, 1.0f };
	Vector4 markerTipColor_{ 0.2f, 1.0f, 0.2f, 1.0f };

	// Trail
	Trail trail_;
	Trail::Config trailCfg_{};
	bool emitTrail_ = true;

	// Trail更新用VP（Trail::DrawがvpMatrixを受け取らない設計なのでUpdateで渡す）
	Matrix4x4 lastVp_{ Matrix4x4::Identity() };

	// Json
	JsonManager json_;

	// UI作業用
	char texturePathBuf_[256]{};
	bool requestRebuildTrail_ = false;
};