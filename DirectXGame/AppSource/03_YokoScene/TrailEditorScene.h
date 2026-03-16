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
	struct editDataUnit
	{
		std::string name; // 末端フォルダ名（例: Sword）
		std::unique_ptr<SHEngine::RenderObject> render; // モデル描画用RenderObject
		int modelHandle; // モデルデータのハンドル（保存/ロード用）
		int textureIndex; // テクスチャインデックス（保存/ロード用）
	};

private:
	// Assets/Model/Item/Weapon配下モデルファイル走査&ロードしてリストアップ
	void BuildModelList();
	// リストからindex番目のモデルからRenderObject作成
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

	// 選択モデルインデックス
	int selectedModelIndex_ = -1;

	// モデルデータ
	std::vector<std::unique_ptr<editDataUnit>> modelRenders_;
	Transform modelTransform_{};
	Matrix4x4 modelWvp_{};

	// マーカー（Cube）
	std::unique_ptr<SHEngine::RenderObject> markerOriginRender_;
	std::unique_ptr<SHEngine::RenderObject> markerTipRender_;
	NodeModelData markerModelData_{};

	Vector3 originLocal_{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal_{ 0.0f, 0.55f, -3.2f };

	Vector3 originWS_{};
	Vector3 tipWS_{};

	// Trail
	Trail trail_;
	Trail::Config trailConfig_{};
	bool emitTrail_ = true;

	// Json
	JsonManager json_;

	// UI作業用
	char texturePathBuf_[256]{};
	bool requestRebuildTrail_ = false;
};