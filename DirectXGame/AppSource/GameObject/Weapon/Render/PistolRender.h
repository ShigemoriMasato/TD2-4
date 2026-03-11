#pragma once
#include "GameObject/Weapon/IWeaponRender.h"

class PistolRender : public IWeaponRender {
public:
	// 初期化関数
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, const std::string& filepath) override;

	// 更新関数
	void Update(Matrix4x4 vpMatrix, Vector3 playerPos) override;

	// 描画関数
	void Draw(CmdObj* cmdObj) override;
};
