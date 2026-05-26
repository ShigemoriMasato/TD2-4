#pragma once
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <SHEngine.h>

class EnemyRainManager {
public:
	void Initilaize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime, DirectionalLight dirLight);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	int textureIndex_ = 0;
	static const int kInstanceNum = 10;
	Transform transforms_[kInstanceNum];
	Vector4 colors_[kInstanceNum];
	float fallSpeed_ = 3.0f;
	float posYLimit_ = -20.0f;
	Vector2 widthRange_;
	Vector2 heightRange_;
};
