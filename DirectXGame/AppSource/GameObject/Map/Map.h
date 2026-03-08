#pragma once
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <memory>
#include <vector>
#include <array>

using CmdObj = SHEngine::Command::Object;

/// @brief ゲームマップを管理するクラス
class Map {
public:
	Map();
	~Map() = default;

	// @brief マップの初期化処理
	void Initialize(SHEngine::DrawDataManager* drawDataManager = nullptr, SHEngine::ModelManager* modelManager = nullptr, float minX = 0.0f, float maxX = 38.0f, float minZ = 0.0f, float maxZ = 38.0f);

	// @brief マップの更新処理
	void Update(const Matrix4x4& vpMatrix);

	// @brief マップの描画処理
	void Draw(CmdObj* cmdObj);

	// @brief 指定した座標がマップの境界内にあるか判定
	bool IsInBounds(const Vector3& position) const;

	// @brief 座標をマップの境界内に制限する
	Vector3 ClampToBounds(const Vector3& position) const;

	// @brief マップのx方向の最小値を取得
	float GetMinX() const { return minX_; }

	// @brief マップのx方向の最大値を取得
	float GetMaxX() const { return maxX_; }

	// @brief マップのz方向の最小値を取得
	float GetMinZ() const { return minZ_; }

	// @brief マップのz方向の最大値を取得
	float GetMaxZ() const { return maxZ_; }

	// @brief マップのx方向の幅を取得
	float GetWidth() const { return maxX_ - minX_; }

	// @brief マップのz方向の奥行きを取得
	float GetDepth() const { return maxZ_ - minZ_; }

private:

	// x方向の最小値
	float minX_ = 0.0f;
	// x方向の最大値
	float maxX_ = 38.0f;

	// z方向の最小値
	float minZ_ = 0.0f;
	// z方向の最大値
	float maxZ_ = 38.0f;

	// モデル管理用
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;
	
	// グラスブロックのモデルID
	int grassModelID_ = -1;
	
	// テクスチャインデックス
	int textureIndex_ = 0;
	
	// 描画用レンダーオブジェクト
	std::unique_ptr<SHEngine::RenderObject> render_;

	// マップのグリッドサイズ
	static constexpr int kMapWidth = 20;  // 20ブロック
	static constexpr int kMapDepth = 20;  // 20ブロック
	static constexpr int kMaxInstances = 2048;  // 最大インスタンス数
	
	// ブロックのサイズと間隔
	static constexpr float kBlockSize = 2.0f;  // モデルサイズ2x2x2
	
	// ブロックのスケール
	Vector3 blockScale_ = { 1.0f, 1.0f, 1.0f };
};