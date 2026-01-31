#pragma once
#include<vector>
#include"Object/OreHome.h"
#include"GameCamera/CameraController.h"
#include"UI/Object/SpriteObject.h"

class HomeManager {
public:

	struct HomeMarkUI {
		std::unique_ptr<SpriteObject> arrowObject_;
	};

	void Initialize(const DrawData& goldOreDrawData, int texture, std::vector<Vector3> list, DrawData spriteDrawData);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void DrawUI(Window* window, const Matrix4x4& vpMatrix);

	/// <summary>
	/// 拠点の追加
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rotY">回転</param>
	void AddHome(const Vector3& pos, const float rotY = 0.0f);

	/// <summary>
	/// アニメーションさせる
	/// </summary>
	/// <param name="pos"></param>
	void SetAnimation(const Vector3& pos);

	// カメラクラスを設定
	void SetCamera(CameraController* cameraController) {
		cameraController_ = cameraController;
	}

	void AddMark(const Vector3& pos);

private:

	// カメラクラス
	CameraController* cameraController_ = nullptr;

	DrawData drawData_;
	int texture_;

	// 拠点の存在するリスト
	std::vector<std::unique_ptr<OreHome>> homeList_;

	// 背景画像
	std::vector<HomeMarkUI> homeMarkList_;

	int32_t activeIndex_ = 0;
};