#pragma once
#include <Utility/MatrixFactory.h>
#include <memory>

//初期値を画面サイズ1280*720と仮定して設定

/**
 * @struct PerspectiveFovDesc
 * @brief 透視投影の設定パラメータ
 */
struct PerspectiveFovDesc {
	/// @brief 視野角Y（ラジアン）
	float fovY = 0.45f;
	/// @brief アスペクト比
	float aspectRatio = 1280.0f / 720.0f;
	/// @brief ニアクリップ面
	float nearClip = 0.1f;
	/// @brief ファークリップ面
	float farClip = 1000.0f;
};

/**
 * @struct OrthographicDesc
 * @brief 正射影投影の設定パラメータ
 */
struct OrthographicDesc {
	/// @brief 左端
	float left = -0.0f;
	/// @brief 上端
	float top = 0.0f;
	/// @brief 右端
	float right = 1280;
	/// @brief 下端
	float bottom = 720;
	/// @brief ニアクリップ面
	float nearClip = 0.0f;
	/// @brief ファークリップ面
	float farClip = 1000.0f;
};

/**
 * @class Camera
 * @brief 3Dカメラを管理するクラス
 * 
 * カメラの位置、回転、スケールを管理し、
 * 透視投影または正射影投影のView-Projection行列を生成する。
 */
class Camera {
public:

	Camera() = default;
	~Camera() = default;
	
	void SetProjectionMatrix(PerspectiveFovDesc desc);
	void SetProjectionMatrix(OrthographicDesc desc);

	//TransformとProjectionMatrixをかけて、VPMatrixを作成する
	virtual void MakeMatrix();
	void DrawImGui();

	void SetTransform(Matrix4x4 mat);

	//VPMatrixを取得
	virtual Matrix4x4 GetVPMatrix() const;

	Matrix4x4 GetTransformMatrix() const { return transformMatrix_; }
	Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }

protected:

	Vector3 position_ = { 0.0f, 0.0f, 0.0f };
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	/// @brief 投影行列（ワールド行列にかけると投影変換される）
	Matrix4x4 projectionMatrix_ = Matrix4x4::Identity();
	/// @brief View-Projection行列（Viewport変換抜き）
	Matrix4x4 vpMatrix_ = Matrix4x4::Identity();

	/// @brief カメラ座標変換行列
	Matrix4x4 transformMatrix_ = Matrix4x4::Identity();

	/// @brief 投影行列が設定済みかどうか
	bool isSetMatrix = false;
};

