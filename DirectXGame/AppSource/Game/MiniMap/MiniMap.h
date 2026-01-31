#pragma once
#include <Camera/Camera.h>
#include <Screen/DualDisplay.h>
#include <Screen/Window.h>
#include <Utility/DataStructures.h>
#include <Render/RenderObject.h>

class MiniMap {
public:

	void Initialize(int mapWidth, int mapHeight, TextureManager* textureManager, const DrawData& plane, const DrawData& visionFrame);
	Camera* PreDraw(Window* window);
	void PostDraw(Window* window, const Matrix4x4& vpMatrix, Vector3 playerPosition, float range);
	void Draw(Window* window);
	void DrawImGui();

	int GetTextureIndex() const { return display_->GetTextureData()->GetOffset(); }

private:

	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<DualDisplay> display_ = nullptr;
	std::unique_ptr<RenderObject> visionField_ = nullptr;
	int vfModelsTextureIndex_ = -1;
	Transform transform_{};

	float distRatio_ = 0.2f;
	float dist = 0.0f;
	float rangeAdjust_ = 20.f;

	std::unique_ptr<RenderObject> miniMapRender_ = nullptr;
};
