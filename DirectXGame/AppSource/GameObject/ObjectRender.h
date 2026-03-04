#pragma once
#include "DrawInfo.h"
#include <Assets/Model/ModelManager.h>
#include <Render/DrawDataManager.h>
#include <Render/RenderObject.h>

class ObjectRender {
public:

	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager);
	void SetDrawInfo(DrawInfo* info, size_t num, Matrix4x4 vpMatrix);
	void Draw(CmdObj* cmdObj);

private:

	void AddRenderObject(int modelID);

	struct Buffer {
		std::vector<Matrix4x4> matrix;
		std::vector<Vector4> color;
	};
	std::map<int, std::unique_ptr<SHEngine::RenderObject>> renderObjects_;
	std::map<int, Buffer> buffers_;

	SHEngine::DrawDataManager* drawDataManager_;
	SHEngine::ModelManager* modelManager_;

};
