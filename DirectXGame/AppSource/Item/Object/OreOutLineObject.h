#pragma once
#include <Render/RenderObject.h>
#include"Render/DrawDataManager.h"
#include"Utility/DataStructures.h"

class OreOutLineObject {
public:

	struct TransformationMatrix {
		Matrix4x4 WVP = Matrix4x4::Identity();
		Matrix4x4 World = Matrix4x4::Identity();
		Matrix4x4 worldInverseTranspose = Matrix4x4::Identity();
	};

	struct Material {
		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
		int textureIndex = 0;
		float shininess = 250.0f;
		int32_t isActive = false;
		float padding;
	};

	// 初期化
	void Initialize(DrawData drawData);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	void SetDrawData(DrawData drawData) {
		// 描画モデルを設定
		renderObject_->SetDrawData(drawData);
	}

public:

	// srt
	Transform transform_;

	// マテリアル関係
	Material material_{};

private:

	//　オブジェクトデータ
	std::unique_ptr<RenderObject> renderObject_;

	// worldTransform関係
	TransformationMatrix vsData_{};
	int vsDataIndex_ = -1;
	Matrix4x4 worldMatrix_;
	
	int psDataIndex_ = -1;

	// ライト
	int lightDataIndex_ = -1;
};