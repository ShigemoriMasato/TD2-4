#include "ShotgunRender.h"

void ShotgunRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, const std::string& filepath) {
	IWeaponRender::Initialize(drawDataManager, modelManager, filepath);
}

void ShotgunRender::Update(Matrix4x4 vpMatrix, Vector3 playerPos) { IWeaponRender::Update(vpMatrix, playerPos); }

void ShotgunRender::Draw(CmdObj* cmdObj) { IWeaponRender::Draw(cmdObj); }
