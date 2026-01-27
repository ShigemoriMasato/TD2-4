#pragma once
#include <Render/RenderObject.h>
#include"Render/DrawDataManager.h"
#include"Utility/DataStructures.h"

class SpriteObject {
public:

	// 定数バッファ
	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 uvMatrix;
		Matrix4x4 WVP;
	};

	~SpriteObject();

	// 初期化
	void Initialize(DrawData drawData,const Vector2& size);

	// 更新処理
	void Update();

	// 描画
	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	// srt
	Transform transform_;

	// 色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

private:

	// uv
	//Transform uvTransform_;

	//　オブジェクトデータ
	std::unique_ptr<RenderObject> renderObject_;

	// worldTransform関係
	ConstBufferData cBuffData_{};
	int cBuffDataIndex_ = -1;
	int pcBuffDataIndex_ = -1;
	Matrix4x4 worldMatrix_;
};