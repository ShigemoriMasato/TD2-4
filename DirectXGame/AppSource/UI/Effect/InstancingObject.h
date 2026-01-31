#pragma once
#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include"Utility/DataStructures.h"
#include<vector>

class InstancingObject {
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
	void Initialize(DrawData spriteDrawData);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);


public:

	// 各要素のトランスフォーム
	std::vector<TransformData> transformDatas_;

	// 数を数える
	int32_t index_ = 0;

	int texture_ = 0;

private:

	// 描画の最大数
	uint32_t maxNum_ = 50;

	//　オブジェクトデータ
	std::unique_ptr<RenderObject> renderObject_;
	std::vector<ParticleForGPU> instancingData_;

	int vsDataIndex_ = -1;
	Matrix4x4 worldMatrix_;
};