#pragma once
#include <Utility/Matrix.h>
#include <Utility/Vector.h>

struct Ray {
	Vector3 origin;
	Vector3 direction;

	// Y座標を指定してXZ平面上の座標を取得
	std::optional<Vector3> GetXZ(float y)
	{
		// direction.y が 0 の場合、XZ 平面と交差しない（平行）
		if (direction.y == 0.0f) return std::nullopt;

		// origin.y + t * direction.y = y を解く
		float t = (y - origin.y) / direction.y;

		// レイ上の点を求める
		Vector3 p = {
			origin.x + t * direction.x,
			y, // Y は指定値
			origin.z + t * direction.z
		};

		return p;
	}

};

/// <summary>
/// レイキャスト関連の処理をまとめたクラス
/// </summary>
class ScreenRaycaster {
public:
	// コンストラクタ
	ScreenRaycaster(float screenWidth, float screenHeight);
	~ScreenRaycaster() {};

	// カメラの設定
	void SetCamera(const Matrix4x4& view, const Matrix4x4& proj);
	void SetInverseVP(const Matrix4x4& invVP) { invVP_ = invVP; }

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
