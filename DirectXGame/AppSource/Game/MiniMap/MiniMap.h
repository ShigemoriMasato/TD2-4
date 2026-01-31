#pragma once
#include <Camera/Camera.h>
#include <Screen/DualDisplay.h>
#include <Screen/Window.h>
#include <Utility/DataStructures.h>
#include <Render/RenderObject.h>
#include <functional>

class MiniMap {
public:

	void Initialize(int mapWidth, int mapHeight, TextureManager* textureManager, const DrawData& plane, const DrawData& visionFrame);
	void Update();
	Camera* PreDraw(Window* window);
	void PostDraw(Window* window, const Matrix4x4& vpMatrix, Vector3 playerPosition, float range);
	void Draw(Window* window);
	void DrawImGui();

	bool PleasePose() const { return pleasePose_; }

	int GetTextureIndex() const { return display_->GetTextureData()->GetOffset(); }

	// クリックした時の時間の制御
	void SetOnClicked(std::function<void()> cb) { onClicked_ = std::move(cb); }

private:

	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<DualDisplay> display_ = nullptr;
	int vfModelsTextureIndex_ = -1;
	Transform transform_{};

	float distRatio_ = 0.2f;
	float dist = 0.0f;

	std::unique_ptr<RenderObject> miniMapRender_ = nullptr;

	// クリックコールバック
	std::function<void()> onClicked_ = nullptr;

private:// 幕系
	
	std::unique_ptr<RenderObject> visionField_ = nullptr;
	float rangeAdjust_ = 20.f;

private:// Pose系
	
	float vfScaleForPose_ = 1.0f;
	Vector2 screenMousePos_ = {};
	bool pleasePose_ = false;

};
