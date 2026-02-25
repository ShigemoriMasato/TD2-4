#pragma once
#include <Utility/Matrix.h>
#include <Utility/Vector.h>

struct Ray {
	Vector3 origin;
	Vector3 direction;
};

/// <summary>
/// レイキャスト関連の処理をまとめたクラス
/// </summary>
class ScreenRaycaster {
public:
	// コンストラクタ
	ScreenRaycaster(float screenWidth, float screenHeight);

	// カメラの設定
	void SetCamera(const Matrix4x4& view, const Matrix4x4& proj);

	// スクリーン座標からカメラ座標への変換
	Vector2 ScreenToNDC(float screenX,float screenY);

	// スクリーン座標からワールド座標への変換
	Vector3 ScreenToWorldGround(float screenX, float screenY);

	// スクリーン座標からレイへの変換
	Ray ScreenToRay(float screenX, float screenY);

	// 行列変換関数
	Vector4 Transform(Vector4& vector, Matrix4x4& matrix);

private:
	float screenWidth_{};  // 画面横幅
	float screenHeight_{}; // 画面縦幅

	Matrix4x4 view_{};  // ビュー行列
	Matrix4x4 proj_{};  // 射影行列
	Matrix4x4 invVP_{}; // ビュープロジェクション逆行列
};
