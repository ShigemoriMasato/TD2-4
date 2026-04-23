#pragma once
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <memory>
#include <vector>
#include <array>
#include "MapInfo.h"

using CmdObj = SHEngine::Command::Object;

/// @brief ゲームマップを管理するクラス
class Map {
public:
	Map();
	~Map() = default;

	// @brief マップの初期化処理
	void Initialize(SHEngine::DrawDataManager* drawDataManager = nullptr, SHEngine::ModelManager* modelManager = nullptr, const MapInfo& mapInfo = MapInfo{},std::string Path = "Assets/Model/Stage");

	// @brief マップの更新処理
	void Update(const Matrix4x4& vpMatrix, float deltaTime = 0.0f);

	// @brief マップの描画処理
	void Draw(CmdObj* cmdObj);

	// @brief ImGuiでのデバッグ表示
	void DrawDebugGUI();

	// @brief 指定した座標がマップの境界内にあるか判定
	bool IsInBounds(const Vector3& position) const;

	// @brief 座標をマップの境界内に制限する
	Vector3 ClampToBounds(const Vector3& position) const;
	
	const MapInfo& GetMapInfo() const { return mapInfo_; }

	// @brief マップのx方向の最小値を取得
	float GetMinX() const { return mapInfo_.minX; }

	// @brief マップのx方向の最大値を取得
	float GetMaxX() const { return mapInfo_.maxX; }

	// @brief マップのz方向の最小値を取得
	float GetMinZ() const { return mapInfo_.minZ; }

	// @brief マップのz方向の最大値を取得
	float GetMaxZ() const { return mapInfo_.maxZ; }

	// @brief マップのx方向の幅を取得
	float GetWidth() const { return mapInfo_.maxX - mapInfo_.minX; }

	// @brief マップのz方向の奥行きを取得
	float GetDepth() const { return mapInfo_.maxZ - mapInfo_.minZ; }

	// @brief ステージの自動Y軸回転を有効化
	void EnableStageAutoRotation(bool enable) { enableAutoRotation_ = enable; }

	void SetStageTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale) {
		stagePosition_ = position;
		stageRotation_ = rotation;
		stageScale_ = scale;
	}
private:
	MapInfo mapInfo_;

	// モデル管理用
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	
	// グラスブロックのモデルID
	int grassModelID_ = -1;

	// テクスチャインデックス
	int textureIndex_ = 0;

	// 描画用レンダーオブジェクト
	std::unique_ptr<SHEngine::RenderObject> render_;

	// ステージモデルのモデルID
	int stageModelID_ = -1;

	// ステージのテクスチャインデックス
	int stageTextureIndex_ = 0;

	// ステージの描画用レンダーオブジェクト
	std::unique_ptr<SHEngine::RenderObject> stageRender_;

	// ステージのトランスフォーム
	Vector3 stagePosition_ = { 24.0f, -0.5f, 24.0f };
	Vector3 stageRotation_ = { 0.0f, 0.0f, 0.0f };
	Vector3 stageScale_ = { 5.0f, 5.0f, 5.0f };

	// 自動回転用の変数
	bool enableAutoRotation_ = false;
	float rotationTimer_ = 0.0f;
	float baseRotationSpeed_ = 0.25f; // 1回転/秒（2π rad/s）
	static constexpr float kNormalDuration = 5.0f;   // 通常速度の時間
	static constexpr float kAccelDuration = 1.0f;    // 加速の時間
	static constexpr float kDecelDuration = 1.0f;    // 減速の時間
	static constexpr float kTotalCycleDuration = kNormalDuration + kAccelDuration + kDecelDuration;

	// マップのグリッドサイズ
	static constexpr int kMapWidth = 25;  // 20ブロック
	static constexpr int kMapDepth = 25;  // 20ブロック
	static constexpr int kMaxInstances = 4096;  // 最大インスタンス数

	// ブロックのサイズと間隔
	static constexpr float kBlockSize = 2.0f;  // モデルサイズ2x2x2

	// ブロックのスケール
	Vector3 blockScale_ = { 1.0f, 1.0f, 1.0f };

	DirectionalLight dirLight_;
};