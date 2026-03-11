#include "PistolRender.h"

void PistolRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, const std::string& filepath) {
	IWeaponRender::Initialize(drawDataManager,modelManager,filepath);
}

void PistolRender::Update(Matrix4x4 vpMatrix, Vector3 playerPos) {
	IWeaponRender::Update(vpMatrix, playerPos);
}

void PistolRender::Draw(CmdObj* cmdObj) {
	IWeaponRender::Draw(cmdObj);
}
