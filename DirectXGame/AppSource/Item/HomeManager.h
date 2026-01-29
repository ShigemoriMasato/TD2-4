#pragma once
#include<vector>
#include"Object/OreHome.h"

class HomeManager {
public:

	void Initialize(const DrawData& goldOreDrawData, int texture, std::vector<Vector3> list);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	/// <summary>
	/// 拠点の追加
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rotY">回転</param>
	void AddHome(const Vector3& pos, const float rotY = 0.0f);

private:

	DrawData drawData_;
	int texture_;

	// 拠点の存在するリスト
	std::vector<std::unique_ptr<OreHome>> homeList_;
};