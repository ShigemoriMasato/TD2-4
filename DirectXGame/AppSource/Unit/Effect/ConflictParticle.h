#pragma once
#include<vector>
#include"UI/Effect/InstancingObject.h"

class CollectParticle {
public:

	// 1つのパーティクルがもつデータ
	struct ParticleData {
		Transform transform; // トラスフォーム
		Vector4 color;       // 色
		float lifeTime;      // 生存時間
		float currentTime;   // 現在の時間
		uint32_t textureHandle = 0; // 使用するテクスチャ

		float rotSpeed;
		float speed;
		float startScale;
	};

	// 初期化
	void Initialize(DrawData drawData);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	Vector3 pos_;

	int texture_ = 0;

private: // パーティクル

	//　オブジェクトデータ
	std::unique_ptr<InstancingObject> instancingObject_;

	// パーティクルデータ
	std::vector<ParticleData> particleDatas_;

	// 描画の最大数
	uint32_t maxNum_ = 50;

	// 数を数える
	int32_t index_ = 0;

	float timer_ = 0.0f;
	float coolTime_ = 0.1f;

private:

	ParticleData MakeNewData();

	void Create();

	void Move();
};