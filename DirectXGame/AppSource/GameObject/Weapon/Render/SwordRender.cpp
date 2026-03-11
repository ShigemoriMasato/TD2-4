#include "SwordRender.h"

void SwordRender::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, IWeapon* weapon, Item itemData) {
	IWeaponRender::Initialize(drawDataManager, modelManager, weapon, itemData);
}

void SwordRender::Update(Matrix4x4 vpMatrix, Vector3 playerPos) { 
	IWeaponRender::Update(vpMatrix, playerPos); 
}

void SwordRender::Draw(CmdObj* cmdObj) { 
	IWeaponRender::Draw(cmdObj); 
}
