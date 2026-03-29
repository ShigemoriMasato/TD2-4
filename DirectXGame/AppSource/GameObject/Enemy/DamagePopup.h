#pragma once
#include <Render/Font/Text.h>
#include <GameObject/DrawInfo.h>
#include <SHEngine.h>
#include <memory>
#include <string>

class DamagePopup {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	
	void Show(int damage, const Vector3& position);
	void Update(float deltaTime, Matrix4x4 vpMatrix);
	
	std::vector<DrawInfo> GetDrawInfo();
	bool IsActive() const { return isActive_; }
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::Text> text_;
	DrawInfo planeDrawInfo_;
	
	Vector3 position_;
	Vector3 rotate_;
	float lifetime_ = 0.0f;
	float maxLifetime_ = 1.0f;
	bool isActive_ = false;
	
	Matrix4x4 vpMatrix_;
	
	SHEngine::ModelManager* modelManager_ = nullptr;
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
};
