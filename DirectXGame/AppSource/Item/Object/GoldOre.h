#pragma once
#include"Collision/Collider.h"

#include"OreItem.h"
#include"OreObject.h"

class GoldOre : public OreItem,public Collider {
public:

	void Init(DrawData drawData, const Vector3& pos);

	void Update() override;

	void Draw(Window* window, const Matrix4x4& vpMatrix) override;

	void OnCollision(Collider* other) override;

public:

	// タイプ
	OreType GetType() override { return OreType::Gold; }

	// 位置
	Vector3 GetPos() override { return object_->transform_.position; }

	// サイズ
	Vector3 GetSize() override { return size_; }

	// 死亡フラグ
	bool IsDead() override { return isDead_; }

	// 満員かどうか
	int32_t IsFullWorker(const int32_t& num) override {
		return maxWorkerNum_ - (num + currentWorkerNum_);
	}

	// 労働者を増やす
	void AddWorker() override {
		currentWorkerNum_++;
	}

	// 労働者を減らす
	void RemoveWorker() override {
		if (currentWorkerNum_ > 0) {
			currentWorkerNum_--;
		}
	}

	// デバック情報
	int32_t GetCurrentWorkerNum() override { return currentWorkerNum_; }
	int32_t GetMaxWorkerNum() override { return maxWorkerNum_; }
	int32_t GetMaxHp() override { return hp_; }
	int32_t GetHp() override { return maxHp_; }

private:
	// オブジェクトデータ
	std::unique_ptr<OreObject> object_;

	// 死亡フラグ
	bool isDead_ = false;

	// 体力
	int32_t hp_ = 0;

	Vector3 size_ = {3.0f,3.0f,3.0f};

	// 円の当たり判定
	Quad quadCollider_;

	// デバック用
	std::string kGroupName_ = "GoldOreItem";

	// 現在向かっている/作業中の数
	int32_t currentWorkerNum_ = 0;
	// 最大の数
	int32_t maxWorkerNum_ = 5;

private: // 調整項目

	int32_t maxHp_ = 10;

private:

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();
};