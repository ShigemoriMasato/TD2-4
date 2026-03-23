#pragma once
#include <GameObject/Effect/Trail/Trail.h>

#include <Render/RenderObject.h>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <assets/Model/ModelManager.h>

#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/DataStructures.h>

class TestTrail4
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		int swordModelHandle);
	void Update(float deltaTime, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Trigger();

	void Stop();

private:
	// 外部
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	NodeModelData swordModelData_;

	// Trail
	Trail trail_;
	Vector3 Origin_ = {};
	Vector3 tip_ = {};

	int vertexOriginIndex_ = 0;
	int vertexTipIndex_ = 1;


	// 剣描画
	std::unique_ptr<SHEngine::RenderObject> render_;
	int textureIndex_;
	Transform transform_ = {};
	Matrix4x4 wvp_ = {};

	// 状態
	bool active_ = false;
	bool emitting_ = false;
	float time_ = 0.0f;
};