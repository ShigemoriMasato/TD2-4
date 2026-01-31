#pragma once
#include"Utility/Vector.h"
#include"Render/DrawDataManager.h"
#include <Render/RenderObject.h>

enum class OreType {
	Large,  // 大
	Medium, // 中
	Small,  // 小

	MaxCount // 数
};

class OreItem {
public:
	virtual ~OreItem() = default;

	// 更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw(Window* window, const Matrix4x4& vpMatrix) = 0;

	// タイプ
	virtual OreType GetType() = 0;

	// 位置
	virtual Vector3 GetPos() = 0;

	// サイズ
	virtual Vector3 GetSize() = 0;

	// 破壊されたか
	virtual bool IsDead() = 0;

	// 労働者を追加できるか確認
	virtual int32_t IsFullWorker(const int32_t& num) = 0;
	// 労働者を増やす
	virtual void AddWorker() = 0;
	// 労働者を減らす
	virtual void RemoveWorker() = 0;

	// 現在の数
	virtual int32_t GetCurrentWorkerNum() = 0;
	// 最大の数
	virtual int32_t GetMaxWorkerNum() = 0;
	virtual int32_t GetMaxHp() = 0;
	virtual int32_t GetHp() = 0;
	virtual float GetRotY() = 0;
};