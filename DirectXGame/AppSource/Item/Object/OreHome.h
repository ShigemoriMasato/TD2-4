#pragma once
#include"GameCommon/DefaultObject.h"

class OreHome {
public:

	void Initialize(const DrawData& homeDrawData, int texture,const Vector3& pos,const float& rotY);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void StartAnimation() {
		if (!isAnimation_) {
			isAnimation_ = true;
		}
	}

	// 家のオブジェクト
	std::unique_ptr<DefaultObject> homeObject_;

private:

	

	bool isAnimation_ = false;

	float timer_ = 0.0f;

	float maxTime_ = 0.5f;

private:

	// アニメーション
	void Animation();
};