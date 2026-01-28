#pragma once
#include <Render/RenderObject.h>
#include <Game/NewMap/NewMap.h>

//MapChipをDebug表示するクラス
class DebugMCRender {
public:

	void Initialize(const DrawData& drawData);
	void Update();
	void Draw(const Matrix4x4& vpMatrix, std::map<TileType, Vector3> colorMap, const MapChipData& mcData, Window* window);

	void SetAlpha(float alpha) { alpha_ = alpha; };

private:

	struct MatrixData {
		Matrix4x4 world;
		Matrix4x4 vp;
	};
	std::vector<std::vector<MatrixData>> matrixData_;
	std::vector<std::vector<Matrix4x4>> worldMatrices_;
	std::vector<MatrixData> forGPUData_;
	std::vector<Vector4> colors;

	float alpha_ = 1.0f;

	int maxSize_ = 128;

	std::unique_ptr<RenderObject> render_ = nullptr;
};
