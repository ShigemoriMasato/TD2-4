#pragma once
#include"Camera/Camera.h"
#include"Input/Input.h"

class CameraController : public Camera {
public:

	void Initialize(Input* input);

	void Update();

	void DebugDraw();

	/// <summary>
	/// マウスのスクリーン座標を3D空間の座標に変換
	/// </summary>
	/// <param name="screenPos">マウスのスクリーン座標</param>
	/// <param name="screenWidth">スクリーンの幅</param>
	/// <param name="screenHeight">スクリーンの高さ</param>
	/// <returns>3D空間上の座標</returns>
	Vector3 ScreenToWorld(const Vector2& screenPos, float screenWidth = 1280.0f, float screenHeight = 720.0f) const;

public:

	Matrix4x4 GetVpMatrix() const { return vpMatrix_; }

	Vector3 GetWorldPos() { return worldPos_; }

	// カメラのワールド座標位置を取得
	Vector3 GetCameraWorldPos() {
		// ワールド座標を入れる変数
		Vector3 worldPos;
		// ワールド行列の平行移動成分を取得
		worldPos.x = worldMatrix_.m[3][0];
		worldPos.y = worldMatrix_.m[3][1];
		worldPos.z = worldMatrix_.m[3][2];
		return worldPos;
	}

private:

	void MakeMatrix() override;


	Vector3 backDir_ = { 0.0f,-0.9f,0.4f };
	float backDist_ = 10.0f;

	Input* input_ = nullptr;
	//クリックした瞬間のマウス座標
	Vector3 clickedCameraPos_ = {};
	Vector2 mousePos_ = {};
	bool preClicked_ = false;
	float deadZone_ = 10.0f;

	// なぜかカメラのワールド行列がないので追加
	Matrix4x4 worldMatrix_;

	// 移動速度
	float moveSpeed_ = 30.0f;

	// ワールド座標の位置
	Vector3 worldPos_ = {};
};