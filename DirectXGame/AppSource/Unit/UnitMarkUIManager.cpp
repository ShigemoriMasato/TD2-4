#include"UnitMarkUIManager.h"
#include<numbers>
#include <Common/DebugParam/GameParamEditor.h>

void UnitMarkUIManager::Initialize(DrawData spriteDrawData, int texture, int iconTex, int playerTex) {

	spriteDrawData_ = spriteDrawData;

    arrowTex_ = texture;
    iconTex_ = iconTex;

    offsetRot_ = -std::numbers::pi_v<float> / 2.0f;

#ifdef USE_IMGUI
    RegisterDebugParam();
#endif
    ApplyDebugParam();

	markUIList_.resize(maxNum_);
	for (size_t i = 0; i < maxNum_; ++i) {
		markUIList_[i].arrowObject_ = std::make_unique<SpriteObject>();
		markUIList_[i].arrowObject_->Initialize(spriteDrawData, { conflictSize_,conflictSize_ });
		markUIList_[i].arrowObject_->transform_.position = { -128.0f,0.0f,0.0f };
		markUIList_[i].arrowObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
        markUIList_[i].arrowObject_->SetTexture(arrowTex_);
        // icon
        markUIList_[i].iconObject_ = std::make_unique<SpriteObject>();
        markUIList_[i].iconObject_->Initialize(spriteDrawData, { conflictSize_,conflictSize_ });
        markUIList_[i].iconObject_->transform_.position = { -128.0f,0.0f,0.0f };
        markUIList_[i].iconObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
        markUIList_[i].iconObject_->SetTexture(iconTex_);
	}

    // プレイヤー
    playerMarkUI_.arrowObject_ = std::make_unique<SpriteObject>();
    playerMarkUI_.arrowObject_->Initialize(spriteDrawData, { playerSize_,playerSize_ });
    playerMarkUI_.arrowObject_->transform_.position = { -128.0f,0.0f,0.0f };
    playerMarkUI_.arrowObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
    playerMarkUI_.arrowObject_->SetTexture(arrowTex_);
    // icon
    playerMarkUI_.iconObject_ = std::make_unique<SpriteObject>();
    playerMarkUI_.iconObject_->Initialize(spriteDrawData, { playerSize_,playerSize_ });
    playerMarkUI_.iconObject_->transform_.position = { -128.0f,0.0f,0.0f };
    playerMarkUI_.iconObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
    playerMarkUI_.iconObject_->SetTexture(playerTex);
}

void UnitMarkUIManager::Update() {
#ifdef USE_IMGUI
    ApplyDebugParam();
#endif

    // UIを生成する
    ProcessClusters();

	// マークを更新
	if (activeIndex_ != -1) {
		for (size_t i = 0; i <= activeIndex_; ++i) {
			markUIList_[i].arrowObject_->Update();
            markUIList_[i].iconObject_->Update();
		}
	}

    // プレイヤーマーク
    playerMarkUI_.arrowObject_->Update();
    playerMarkUI_.iconObject_->Update();
}

void UnitMarkUIManager::Reset() {
	activeIndex_ = -1;

	// 位置をクリアする
	conflictPosList_.clear();
}

void UnitMarkUIManager::DrawUI(Window* window, const Matrix4x4& vpMatrix) {

    // プレイヤーのUI
    if (isPlayerDraw_) {
        playerMarkUI_.arrowObject_->Draw(window, vpMatrix);
        playerMarkUI_.iconObject_->Draw(window, vpMatrix);
    }

	// 家マークUI
	if (activeIndex_ != -1) {
		for (size_t i = 0; i <= activeIndex_; ++i) {
			markUIList_[i].arrowObject_->Draw(window, vpMatrix);
            markUIList_[i].iconObject_->Draw(window, vpMatrix);
		}
	}
}

void UnitMarkUIManager::AddConflict(const Vector3& pos) {
	// 衝突位置
	conflictPosList_.push_back(pos);
}

void UnitMarkUIManager::SetPlayerPos(const Vector3& pos) {
    MarkerResult marker = cameraController_->GetMarkerInfo(pos, playerMargin_);

    isPlayerDraw_ = !marker.isVisible;

    if (!marker.isVisible) {
        playerMarkUI_.arrowObject_->transform_.position.x = marker.position.x;
        playerMarkUI_.arrowObject_->transform_.position.y = marker.position.y;
        playerMarkUI_.arrowObject_->transform_.rotate.z = marker.rotation + offsetRot_;
        // icon
        playerMarkUI_.iconObject_->transform_.position.x = marker.position.x;
        playerMarkUI_.iconObject_->transform_.position.y = marker.position.y;
    }
}

void UnitMarkUIManager::AddConflictMarkUI(const Vector3& pos) {
	MarkerResult marker = cameraController_->GetMarkerInfo(pos, conflictMargin_);

	if (!marker.isVisible) {

		activeIndex_++;

		if (activeIndex_ < maxNum_) {
			markUIList_[activeIndex_].arrowObject_->transform_.position.x = marker.position.x;
			markUIList_[activeIndex_].arrowObject_->transform_.position.y = marker.position.y;
			markUIList_[activeIndex_].arrowObject_->transform_.rotate.z = marker.rotation + offsetRot_;
            // icon
            markUIList_[activeIndex_].iconObject_->transform_.position.x = marker.position.x;
            markUIList_[activeIndex_].iconObject_->transform_.position.y = marker.position.y;
		} else {
			MarkUI mark;
			mark.arrowObject_ = std::make_unique<SpriteObject>();
			mark.arrowObject_->Initialize(spriteDrawData_, { conflictSize_,conflictSize_ });
			mark.arrowObject_->transform_.position = { marker.position.x,marker.position.y,0.0f };
			mark.arrowObject_->transform_.rotate.z = marker.rotation;
			mark.arrowObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
            mark.arrowObject_->SetTexture(arrowTex_);
            // icon
            mark.iconObject_ = std::make_unique<SpriteObject>();
            mark.iconObject_->Initialize(spriteDrawData_, { conflictSize_,conflictSize_ });
            mark.iconObject_->transform_.position = { marker.position.x,marker.position.y,0.0f };
            mark.iconObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
            mark.iconObject_->SetTexture(iconTex_);

			// マークUIを登録する
			markUIList_.push_back(std::move(mark));

			maxNum_++;
		}
	}
}

void UnitMarkUIManager::ProcessClusters() {
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

    // UIの生成
    for (const auto& cluster : clusters) {
        Vector3 finalPos = {
            cluster.positionSum.x / cluster.count,
            cluster.positionSum.y / cluster.count,
            cluster.positionSum.z / cluster.count
        };

        AddConflictMarkUI(finalPos);
    }
}

void UnitMarkUIManager::RegisterDebugParam() {
    // 登録
    GameParamEditor::GetInstance()->AddItem("MarkUI", "PlayerSize", playerSize_,0);
    GameParamEditor::GetInstance()->AddItem("MarkUI", "PlayerMargin", playerMargin_,1);
    GameParamEditor::GetInstance()->AddItem("MarkUI", "ConflictSize", conflictSize_,2);
    GameParamEditor::GetInstance()->AddItem("MarkUI", "ConflictMargin", conflictMargin_,3);
}

void UnitMarkUIManager::ApplyDebugParam() {
    // 適応
    playerSize_ = GameParamEditor::GetInstance()->GetValue<float>("MarkUI", "PlayerSize");
    playerMargin_ = GameParamEditor::GetInstance()->GetValue<float>("MarkUI", "PlayerMargin");
    conflictSize_ = GameParamEditor::GetInstance()->GetValue<float>("MarkUI", "ConflictSize");  
    conflictMargin_ = GameParamEditor::GetInstance()->GetValue<float>("MarkUI", "ConflictMargin");
}