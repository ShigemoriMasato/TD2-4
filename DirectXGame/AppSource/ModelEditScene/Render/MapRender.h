#pragma once
#include <Render/RenderObject.h>

class MapRender {
public:

	void Initialize(const DrawData& box);
	void Update();
	void Draw(const Matrix4x4& vpMatrix, Window* window);

	void SetAlpha(float alpha) { alpha_ = alpha; };
	void SetConfig(int width, int height, const std::vector<std::vector<int>>& textureIndices);

private:

	struct VSData {
		Matrix4x4 world;
		Matrix4x4 vp;
	};
	std::unique_ptr<RenderObject> render_{};
	std::vector<Matrix4x4> worldMatrices_{};
	std::vector<VSData> vsData_{};
	std::vector<Vector4> colors_{};
	std::vector<int> textureIndices_{};

	float alpha_ = 1.0f;
	const int maxSize_ = 128;
};
