#pragma once
#include <Camera/Camera.h>
#include <Screen/DualDisplay.h>
#include <Screen/Window.h>
#include <Utility/DataStructures.h>
#include <Render/RenderObject.h>
#include <functional>

#include"GameCamera/MarkResult.h"

class MiniMap {
public:

	void Initialize(int mapWidth, int mapHeight, DualDisplay* disp, const DrawData& plane, const DrawData& visionFrame, TextureManager* textureManager);
	void Update(bool debug = false);
	Camera* PreDraw(Window* window);
	void PostDraw(Window* window, const Matrix4x4& vpMatrix, Vector3 playerPosition, float range);
	void Draw(Window* window);
	void DrawImGui();

	bool PleasePose() const { return pleasePose_; }

	int GetTextureIndex() const { return display_->GetTextureData()->GetOffset(); }

	// クリックした時の時間の制御
	void SetOnClicked(std::function<void()> cb) { onClicked_ = std::move(cb); }

	/// <summary>
	/// 3D空間の座標をスクリーン座標に変換する
	/// </summary>
	/// <param name="worldPos">変換したいワールド座標</param>
	/// <returns>x,y:スクリーン座標, z:カメラ前方への距離(w)</returns>
	Vector3 WorldToScreen(const Vector3& worldPos) const;
	MarkerResult GetMarkerInfo(const Vector3& targetWorldPos, float margin, float screenWidth = 1280.0f, float screenHeight = 720.0f);

private:

	std::unique_ptr<RenderObject> bg_ = nullptr;
	int bgti_ = -1;

	std::unique_ptr<Camera> camera_ = nullptr;
	DualDisplay* display_ = nullptr;
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
