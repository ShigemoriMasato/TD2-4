#include "ObjectRender.h"

void ObjectRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager) {
	drawDataManager_ = drawDataManager;
	modelManager_ = modelManager;
}

void ObjectRender::SetDrawInfo(DrawInfo* info, size_t num, Matrix4x4 vpMatrix) {
	for (auto& [id, buffer] : buffers_) {
		buffer.matrix.clear();
		buffer.color.clear();
	}

	for (int i = 0; i < num; i++) {
		int id = info[i].modelIndex;
		AddRenderObject(id);

		auto& renderObject = renderObjects_[id];
		Matrix4x4 world = Matrix::MakeAffineMatrix(info[i].scale, Vector3(), info[i].position);
		buffers_[id].matrix.push_back(world * vpMatrix);
		buffers_[id].color.push_back(ConvertColor(info[i].color));
	}

	for (auto& [id, renderObject] : renderObjects_) {
		if (renderObject) {
			renderObject->CopyBufferData(0, buffers_[id].matrix.data(), sizeof(Matrix4x4) * buffers_[id].matrix.size());
			renderObject->CopyBufferData(1, buffers_[id].color.data(), sizeof(Vector4) * buffers_[id].color.size());
			auto modelData = modelManager_->GetNodeModelData(id);
			int textureIndex = modelData.materials[modelData.materialIndex.front()].textureIndex;
			renderObject->CopyBufferData(2, &textureIndex, sizeof(int));
			renderObject->instanceNum_ = (uint32_t)buffers_[id].matrix.size();
		}
	}
}

void ObjectRender::Draw(CmdObj* cmdObj) {
	for (auto& [id, renderObject] : renderObjects_) {
		if (renderObject) {
			renderObject->Draw(cmdObj);
		}
	}
}

void ObjectRender::AddRenderObject(int modelID) {
	if (renderObjects_.find(modelID) != renderObjects_.end()) {
		return;
	}

	auto& render = renderObjects_[modelID];
	render = std::make_unique<SHEngine::RenderObject>("ObjectRender_" + std::to_string(modelID));
	render->Initialize();
	auto model = modelManager_->GetNodeModelData(modelID);
	auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
	render->SetDrawData(drawData);

	render->psoConfig_.vs = "Simples.VS.hlsl";
	render->psoConfig_.ps = "TexColors.PS.hlsl";
	render->CreateSRV(sizeof(Matrix4x4), 1024, ShaderType::VERTEX_SHADER, "WVP");
	render->CreateSRV(sizeof(Vector4), 1024, ShaderType::PIXEL_SHADER, "Color");
	render->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render->SetUseTexture(true);
}
