#pragma once
#include"OreUnitObject.h"
#include"Unit/MapChipField.h"

class OreUnit {
public:

	void Initialize(MapChipField* mapChipField, DrawData drawData,const Vector3& pos);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	// 死亡フラグを取得
	bool IsDead()const { return isDead_; }

private:
	// マップデータ
	MapChipField* mapChipField_ = nullptr;

	// オブジェクトデータ
	std::unique_ptr<OreUnitObject> object_;

	int32_t hp_ = 0;

	bool isDead_ = false;
};