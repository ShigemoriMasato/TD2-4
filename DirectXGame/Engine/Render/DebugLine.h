#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Camera/Camera.h>

class DebugLine {
public:

	DebugLine() = default;
	~DebugLine() = default;

	void Initialize(DrawDataManager* manager, ModelManager* modelManager, Camera* camera);
	void Draw(Window* window);

	void Fresh();

	void AddLine(Skeleton& skeleton, float jointScale = 0.1f);

private:

	DrawDataManager* drawDataManager_ = nullptr;
	Camera* camera_ = nullptr;

	const uint32_t maxDrawNums_ = 1024;
	const uint32_t lineNums_ = 1024;

	std::vector<Vector3> lineVertices_;

	std::vector<Matrix4x4> sphereVSData_;
	std::vector<Matrix4x4> cubeVSData_;

private:

	std::unique_ptr<RenderObject> sphere_;
	uint32_t currentSphereNum_ = 0;

	std::unique_ptr<RenderObject> cube_;
	uint32_t currentCubeNum_ = 0;

	std::unique_ptr<RenderObject> line_;
	uint32_t currentLineNum_ = 0;
	int drawDataIndex_ = -1;
};
