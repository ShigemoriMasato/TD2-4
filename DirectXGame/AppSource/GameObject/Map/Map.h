#pragma once
#include <Utility/Vector.h>

/// @brief ゲームマップを管理するクラス
class Map {
public:
	Map();
	~Map() = default;

	// @brief マップの初期化処理
	void Initialize(float minX = -20.0f, float maxX = 20.0f, float minZ = -20.0f, float maxZ = 20.0f);

	// @brief マップの更新処理
	void Update();

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
	float minX_ = -20.0f;
	// x方向の最大値
	float maxX_ = 20.0f;

	// z方向の最小値
	float minZ_ = -20.0f;
	// z方向の最大値
	float maxZ_ = 20.0f;
};