#pragma once
#include<vector>
#include"InstancingObject.h"
#include"UI/Object/SpriteObject.h"

class CollectEffectUI {
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
	void Initialize(DrawData spriteDrawData, int texture,int lineTexture);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void StartAnimation() {
		if (!isAnimation_) {
			isAnimation_ = true;
		}

		if (!isLineAnimation_) {
			isLineAnimation_ = true;
		}
	}

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

	bool isAnimation_ = false;
	float animeCoolTimer_ = 0.0f;
	float animeCoolTime_ = 1.5f;

private: // スプライト

	// 背景画像
	std::unique_ptr<SpriteObject> lineSpriteObject_;

	bool isLineAnimation_ = false;
	float lineAnimeCoolTimer_ = 0.0f;
	float lineAnimeCoolTime_ = 0.3f;

private:

	ParticleData MakeNewData();

	void Create();

	void Move();

	void LineAnimatoin();
};