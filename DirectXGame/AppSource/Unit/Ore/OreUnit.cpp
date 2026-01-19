#include"OreUnit.h"
#include"FpsCount.h"

#include"Item/Object/GoldOre.h"

OreUnit::OreUnit(MapChipField* mapChipField, DrawData drawData, Vector3* playerPos) {
	// マップデータ
	mapChipField_ = mapChipField;

	// プレイヤー座標を取得
	playerPos_ = playerPos;

	// オブジェクトを初期化
	object_ = std::make_unique<OreUnitObject>();
	object_->Initialize(drawData);

	// 更新状態を設定する
	statesTable_ = {
		[this]() { GoToUpdate(); },
		[this]() { MiningUpdate(); },
		[this]() { ReturnUpdate(); }
	};

	// 各状態のリセット処理を設定する
	resetStatesTable_ = {
		[this]() {},
		[this]() {},
		[this]() {
			// プレイヤーの位置までの経路を取得
			CalculatePath(*playerPos_); 
		}
	};

	// 当たり判定の設定
	circleCollider_.center = object_->transform_.position;
	circleCollider_.radius = 1.0f;

	// 当たり判定の要素を設定
	CollConfig config;
	config.colliderInfo = &circleCollider_;
	config.isActive = true;
	config.ownTag = CollTag::Unit;
	config.targetTag = static_cast<uint32_t>(CollTag::Player) | static_cast<uint32_t>(CollTag::Enemy);
	SetColliderConfig(config);

	// 当たり判定の初期化
	Initialize();
}

void OreUnit::Init(const Vector3& apearPos, const Vector3& targetPos) {

	// 初期位置を設定
	object_->transform_.position = apearPos;

	// フラグをリセット
	isActive_ = true;
	isDead_ = false;

	// hpを設定
	hp_ = maxHp_;

	// 時間をリセット
	timer_ = 0.0f;

	// 状態を設定
	state_ = State::GoTo;
	// 状態のリセット
	resetStatesTable_[static_cast<size_t>(state_)]();

    // 移動する経路を求める
    CalculatePath(targetPos);
}

void OreUnit::Update() {
	if (isDead_) { return; }

	// 切り替え処理
	if (stateRequest_) {
		// 状態を変更
		state_ = stateRequest_.value();
		// 状態によるリセットを呼び出す
		resetStatesTable_[static_cast<size_t>(state_)]();
		// リクエストをクリア
		stateRequest_ = std::nullopt;
	}

	// プレイヤーの状態による更新処理をおこなう
	statesTable_[static_cast<size_t>(state_)]();

    // 移動処理
    Move();

	// 更新処理
	object_->Update();

	// 当たり判定の位置を更新
	circleCollider_.center = object_->transform_.position;

	// 生存時間
	timer_ += FpsCount::deltaTime / damageTime_;

	if (timer_ >= 1.0f) {
		hp_ -= 1;
		timer_ = 0.0f;
	}

	// 体力が0の時、死亡する
	if (hp_ <= 0) {
		isDead_ = true;
	}
}

void OreUnit::Draw(Window* window, const Matrix4x4& vpMatrix) {
	if (isDead_) { return; }

	// 描画
	object_->Draw(window, vpMatrix);
}

void OreUnit::OnCollision(Collider* other) {
	bool isStage = CollTag::Stage == other->GetOwnTag();

	if (!isStage) { return; }

	GoldOre* goldOre = dynamic_cast<GoldOre*>(other);

	if (goldOre) {

	}
}

void OreUnit::GoToUpdate() {

	// 目的地付けば次の状態に切り替える
	if (path_.empty()) {
		stateRequest_ = State::Mining;
	}

	// 鉱石まで移動する
	Move();
}

void OreUnit::MiningUpdate() {

	// 鉱石を採掘


	stateRequest_ = State::Return;
}

void OreUnit::ReturnUpdate() {

	// プレイヤーまで移動すれば終了
	if (path_.empty()) {
		isActive_ = false;
	}

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
		if ((dx * dx + dz * dz) > 0.1f) {
			// 目的地が変わっているので再計算フラグを立てる
			needRecalc = true;
		}
	}

	// 再計算が必要なら実行
	if (needRecalc) {
		CalculatePath(*playerPos_);
	}

	// 移動する
	Move();
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

	// パスに何も存在しなければ早期リターン
	if (path_.empty()) { return; }

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