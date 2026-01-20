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

	// ユニットとの接触回数
	int32_t GetContactNum() const { return contactNum_; }

private:
	// オブジェクトデータ
	std::unique_ptr<OreObject> object_;

	int32_t hp_ = 0;

	Vector3 size_ = {3.0f,3.0f,3.0f};

	// 円の当たり判定
	Quad quadCollider_;

	// ユニットとの接触回数
	int32_t contactNum_ = 0;
};