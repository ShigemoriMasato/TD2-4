#pragma once
#include"GameCommon/DefaultObject.h"
#include<vector>
#include"Unit/Effect/ConflictParticle.h"

class UnitEffectManager {
public:

	void Initialize(DrawData oreItemData,int oreItemTexture, DrawData boxData);

	void Update();

	void PostCollisionUpdate();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void AddOreItem(const Vector3& pos);

	void Reset() {
		index_ = -1; 
		conflictPosList_.clear();
	}

	// 衝突演出を追加
	void AddConflictEffect(const Vector3& pos) {
		unitParticle_->AddConflict(pos);
	}

	// 衝突位置を取得
	void AddConflict(const Vector3& pos) {
		conflictPosList_.push_back(pos);
	}

private:
	DrawData oreItemData_;
	int oreItemTexture_;

	DrawData boxData_;

	std::vector<std::unique_ptr<DefaultObject>> oreItemObjects_;

	// 数
	int32_t index_ = -1;

	std::unique_ptr<ConflictParticle> unitParticle_;

	// 衝突位置を取得
	std::vector<Vector3> conflictPosList_;
	// 一つにまとめる距離
	float mergeThreshold_ = 5.0f;

private:

	struct Cluster {
		Vector3 positionSum; // 位置の合計
		int count;           // まとめた数
	};

	// 位置をまとめる
	void ProcessClusters();
};

// ヘルプ関数
namespace {
	float LengthSq(const Vector3& v);
}