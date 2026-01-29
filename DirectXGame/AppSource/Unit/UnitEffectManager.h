#pragma once
#include"GameCommon/DefaultObject.h"
#include<vector>


class UnitEffectManager {
public:

	void Initialize(DrawData oreItemData,int oreItemTexture);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void AddOreItem(const Vector3& pos);

	void Reset() { index_ = -1; }

private:
	DrawData oreItemData_;
	int oreItemTexture_;

	std::vector<std::unique_ptr<DefaultObject>> oreItemObjects_;

	// 数
	int32_t index_ = -1;
};