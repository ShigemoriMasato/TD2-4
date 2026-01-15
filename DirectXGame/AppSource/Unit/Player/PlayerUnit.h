#pragma once
#include"Unit/MapChipField.h"
#include"PlayerUnitObject.h"
#include"Common/KeyConfig/KeyManager.h"

class PlayerUnit {
public:

	void Initialize(MapChipField* mapChipField, DrawData drawData,const Vector3& pos, KeyManager* keyManager);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

private:

	// マップデータ
	MapChipField* mapChipField_ = nullptr;

	// オブジェクトデータ
	std::unique_ptr<PlayerUnitObject> object_;

	// キーを取得する
	KeyManager* keyManager_ = nullptr;

	int32_t hp_ = 0;

private:

	// 入力処理
	void ProcessMoveInput();
};