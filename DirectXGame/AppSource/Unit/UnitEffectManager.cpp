#include"UnitEffectManager.h"
#include"FpsCount.h"

void UnitEffectManager::Initialize(DrawData oreItemData, int oreItemTexture, DrawData boxData) {

	boxData_ = boxData;

	// メモリを確保
	oreItemObjects_.resize(100);
    conflictPosList_.reserve(40);

	for (int32_t i = 0; i < 100; ++i) {
		oreItemObjects_[i] = std::make_unique<DefaultObject>();
		oreItemObjects_[i]->Initialize(oreItemData, oreItemTexture);
		oreItemObjects_[i]->transform_.scale = { 1.0f,1.0f,1.0f };
		oreItemObjects_[i]->material_.isActive = true;
	}

	// パーティクルを設定する
	unitParticle_ = std::make_unique<ConflictParticle>();
	unitParticle_->Initialize(boxData);
}

void UnitEffectManager::Update() {

    if (isLog_) {
        timer_ += FpsCount::deltaTime / 0.5f;

        if (timer_ >= 1.0f) {
            timer_ = 0.0f;
            isLog_ = false;
        }
    }

	// 更新処理
	for (int32_t i = 0; i < index_+1; ++i) {
		oreItemObjects_[i]->Update();
	}
}

void UnitEffectManager::PostCollisionUpdate() {
    // 衝突位置を取得
    ProcessClusters();

    // 衝突している時
    if (!isLog_) {
        if (!conflictPosList_.empty()) {
            onCollBack_();
            isLog_ = true;
        }
    }
    
    // 演出の更新処理
    unitParticle_->Update();
}

void UnitEffectManager::AddOreItem(const Vector3& pos) {
	index_++;
	oreItemObjects_[index_]->transform_.position = pos;
}

void UnitEffectManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	for (int32_t i = 0; i < index_+1; ++i) {
		oreItemObjects_[i]->Draw(window,vpMatrix);
	}

	// 演出を描画
	unitParticle_->Draw(window, vpMatrix);
}

void UnitEffectManager::ProcessClusters() {
    // 飛ばす
    if (conflictPosList_.empty()) { return; }

    std::vector<Cluster> clusters;

    float thresholdSq = mergeThreshold_ * mergeThreshold_;

    for (const auto& pos : conflictPosList_) {
        bool merged = false;

        // 既存のクラスタに近いかチェック
        for (auto& cluster : clusters) {
            // 現在のクラスタの平均位置を計算
            Vector3 currentCenter = {
                cluster.positionSum.x / cluster.count,
                cluster.positionSum.y / cluster.count,
                cluster.positionSum.z / cluster.count
            };

            // 距離チェック
            Vector3 diff = {
                pos.x - currentCenter.x,
                pos.y - currentCenter.y,
                pos.z - currentCenter.z
            };

            if (LengthSq(diff) <= thresholdSq) {
                // 近いのでグループに加算
                cluster.positionSum.x += pos.x;
                cluster.positionSum.y += pos.y;
                cluster.positionSum.z += pos.z;
                cluster.count++;
                merged = true;
                break;
            }
        }

        // どのグループにも属さない場合は新規作成
        if (!merged) {
            Cluster newCluster;
            newCluster.positionSum = pos;
            newCluster.count = 1;
            clusters.push_back(newCluster);
        }
    }

    // 演出を発生
    for (const auto& cluster : clusters) {
        Vector3 finalPos = {
            cluster.positionSum.x / cluster.count,
            cluster.positionSum.y / cluster.count,
            cluster.positionSum.z / cluster.count
        };
        finalPos.y = 0.0f;
        AddConflictEffect(finalPos);
    }
}

// ヘルプ関数
namespace {
    float LengthSq(const Vector3& v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }
}