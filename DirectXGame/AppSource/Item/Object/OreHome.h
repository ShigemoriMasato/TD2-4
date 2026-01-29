#pragma once
#include"GameCommon/DefaultObject.h"

class OreHome {
public:

	void Initialize(const DrawData& homeDrawData, int texture,const Vector3& pos,const float& rotY);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

private:

	// 家のオブジェクト
	std::unique_ptr<DefaultObject> homeObject_;

private:

	// アニメーション
	void Animation();
};