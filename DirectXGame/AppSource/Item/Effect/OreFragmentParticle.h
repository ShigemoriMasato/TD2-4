#pragma once
#include<vector>
#include"UI/Effect/InstancingObject.h"

class OreFragmentParticle {
public:

	// 1つのパーティクルがもつデータ
	struct ParticleData {
		Transform transform; // トラスフォーム
		Vector3 velocity;    // 速度
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

	// パーティクルを追加
	void AddParticle(const Vector3& pos);

private: // パーティクル

	//　オブジェクトデータ
	std::unique_ptr<InstancingObject> instancingObject_;

	// パーティクルデータ
	std::vector<ParticleData> particleDatas_;

	// 描画の最大数
	uint32_t maxNum_ = 400;

	// 数を数える
	int32_t index_ = 0;

	float minScale_ = 0.08f;
	float maxScale_ = 0.2f;

private:

	ParticleData MakeNewData(const Vector3& pos);

	void Move();

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();
};