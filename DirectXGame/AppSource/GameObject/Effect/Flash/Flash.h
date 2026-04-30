#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class Flash {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	void Trigger();
	bool GetIsActive() const { return isActive_; }
	float GetIntensity() const { return flashIntensity_; }

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	float flashIntensity_ = 0.0f;
	float flashDecaySpeed_ = 2.0f;

	bool isActive_ = false;
};
