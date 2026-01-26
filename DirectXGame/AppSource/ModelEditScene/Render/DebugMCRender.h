#pragma once
#include <Render/RenderObject.h>
#include <Game/MapData/Data/MapData.h>

//MapChipをDebug表示するクラス
class DebugMCRender {
public:

	void Initialize(MapChipData* data, const DrawData& drawData);
	void Update();
	void Draw(const Matrix4x4& vpMatrix, Window* window);

	void EditColor(TileType type, const Vector3& color);
	void SetAlpha(float alpha) { alpha_ = alpha; };
	std::map<TileType, Vector3> GetColorMap() const { return colorMap_; }

private:

	struct MatrixData {
		Matrix4x4 world;
		Matrix4x4 vp;
	};
	std::vector<std::vector<MatrixData>> matrixData_;
	std::vector<MatrixData> forGPUData_;
	std::vector<Vector4> colors;

	std::map<TileType, Vector3> colorMap_;
	float alpha_ = 1.0f;

	int maxSize_ = 128;

	std::unique_ptr<RenderObject> render_ = nullptr;
	MapChipData* mcData_ = nullptr;

};
