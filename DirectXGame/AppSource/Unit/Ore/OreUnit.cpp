#include"OreUnit.h"
#include"FpsCount.h"

void OreUnit::Initialize(MapChipField* mapChipField, DrawData drawData, const Vector3& apearPos, const Vector3& targetPos,Vector3* playerPos) {
	// マップデータ
	mapChipField_ = mapChipField;

	// プレイヤー座標を取得
	playerPos_ = playerPos;

	// オブジェクトを初期化
	object_ = std::make_unique<OreUnitObject>();
	object_->Initialize(drawData);

	// 初期位置を設定
	object_->transform_.position = apearPos;

    // 移動する経路を求める
    CalculatePath(targetPos);
}

void OreUnit::Update() {
	if (isDead_) { return; }

    // 移動処理
    Move();

	// 更新処理
	object_->Update();
}

void OreUnit::Draw(Window* window, const Matrix4x4& vpMatrix) {
	if (isDead_) { return; }

	// 描画
	object_->Draw(window, vpMatrix);
}

void OreUnit::CalculatePath(const Vector3& goal) {
    // メンバ変数のパスをクリア
    path_.clear();

    if (mapChipField_) {
        // マップクラスに経路計算を依頼
        // startPos は現在の自分の位置
        std::vector<Vector3> calculatedPath = mapChipField_->CalculatePath(object_->transform_.position, goal);
        path_ = calculatedPath;
    }

    // もし経路が見つからなかった場合
    if (path_.empty()) {
        // とりあえずゴール地点だけ入れておく
        path_.push_back(goal);
    }
}

void OreUnit::Move() {

	if (state_ == State::Return && playerPos_) {
		// プレイヤーが現在いるマップチップのインデックスと座標を取得
		MapChipField::IndexSet pIdx = mapChipField_->GetMapChipIndexSetByPosition(*playerPos_);
		Vector3 playerGridPos = mapChipField_->GetMapChipPositionByIndex(pIdx.xIndex, pIdx.zIndex);

		// 「現在の目的地（パスの終点）」と「プレイヤーのいるマスの中心」が違うかチェック
		bool needRecalc = false;
		if (!path_.empty()) {
			Vector3 currentDest = path_.back(); // 現在のパスの最終地点

			// 距離の二乗で比較
			float dx = currentDest.x - playerGridPos.x;
			float dz = currentDest.z - playerGridPos.z;
			if ((dx * dx + dz * dz) > 0.01f) {
				// 目的地が変わっているので再計算フラグを立てる
				needRecalc = true;
			}
		}

		// 再計算が必要なら実行
		if (needRecalc) {
			CalculatePath(*playerPos_);
		}
	}

	// 経路がない場合の処理
	if (path_.empty()) {
		if (state_ == State::GoTo) {
			// 目的地に着いたら、プレイヤーの位置に戻る
			state_ = State::Return;

			// プレイヤーの位置までの経路を取得
			CalculatePath(*playerPos_);
		} else if (state_ == State::Return) {
			isDead_ = true;
		}
		return;
	}

	// 次に進むべき座標を取得
	Vector3 nextTarget = path_.front();
	Vector3 currentPos = object_->transform_.position;

	// 次の地点へのベクトルを計算
	Vector3 toTarget = nextTarget - currentPos;
	toTarget.y = 0.0f;

	// XZ平面での距離を計算
	float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

	// 移動処理
	if (distance < 0.1f) {
		// 十分近づいたら座標を確定させて、リストから削除（次の地点へ）
		object_->transform_.position = nextTarget;
		path_.erase(path_.begin());
	} else {
		// 目的地に向かって移動
		// 正規化（長さを1にする）して速度を掛ける
		Vector3 velocity = (toTarget / distance) * speed_;
		object_->transform_.position += velocity * FpsCount::deltaTime;

		// 進行方向を向かせる（Y軸回転）
		if (distance > 0.001f) {
			object_->transform_.rotate.y = std::atan2(velocity.x, velocity.z);
		}
	}
}