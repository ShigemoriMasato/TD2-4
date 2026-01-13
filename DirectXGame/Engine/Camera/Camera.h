#pragma once
#include <Utility/MatrixFactory.h>
#include <memory>

//初期値を画面サイズ1280*720と仮定して設定

struct PerspectiveFovDesc {
	float fovY = 0.45f;
	float aspectRatio = 1280.0f / 720.0f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
};

struct OrthographicDesc {
	float left = -0.0f;
	float top = 0.0f;
	float right = 1280;
	float bottom = 720;
	float nearClip = 0.0f;
	float farClip = 1000.0f;
};

class Camera {
public:

	Camera() = default;
	~Camera() = default;
	
	void SetProjectionMatrix(PerspectiveFovDesc desc);
	void SetProjectionMatrix(OrthographicDesc desc);

	//TransformとProjectionMatrixをかけて、VPMatrixを作成する
	void MakeMatrix();
	void DrawImGui();

	void SetTransform(Matrix4x4 mat);

	//VPMatrixを取得
	Matrix4x4 GetVPMatrix() const;

	Vector3 position_ = { 0.0f, 0.0f, 0.0f };
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	Matrix4x4 GetTranformMatrix() const { return transformMatrix_; }
	Matrix4x4 GetProjectionMatrix() const { return projectionMatrix; }

protected:

	Matrix4x4 projectionMatrix = Matrix4x4::Identity();		//ワールド行列にこれをかけると正射影になる
	Matrix4x4 vpMatrix = Matrix4x4::Identity();				//w抜きviewport変換行列

	Matrix4x4 transformMatrix_ = Matrix4x4::Identity();		//カメラ座標変換行列

	bool isSetMatrix = false;
};
