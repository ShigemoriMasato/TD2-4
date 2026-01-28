#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include"Utility/DataStructures.h"
#include<vector>

class OreUnitHPUI {
public:

	// 1つのパーティクルがもつデータ
	struct TransformData {
		Matrix4x4 worldMatrix;
		Transform transform;
		Vector4 color;
		uint32_t textureHandle;
	};

	struct alignas(16) ParticleForGPU {
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
		uint32_t textureHandle;
		float padding[3];
	};

	// 初期化
	void Initialize(DrawData wallDrawData);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// 追加
	void Add(const Vector3& pos, const int32_t& hp, const int32_t& maxHp);

private:

	// 各要素のトランスフォーム
	std::vector<TransformData> transformDatas_;

	// 描画の最大数
	uint32_t maxNum_ = 300;

	// 数を数える
	int32_t index_ = 0;

	float rotX_ = -2.4f;

	Vector3 baseScale_ = { 1.0f,0.2f,1.0f };

private: // 描画に使用

	//　オブジェクトデータ
	std::unique_ptr<RenderObject> renderObject_;
	std::vector<ParticleForGPU> instancingData_;

	int vsDataIndex_ = -1;
	Matrix4x4 worldMatrix_;

	// 実際に描画する数
	uint32_t currentNum_ = 0;
};