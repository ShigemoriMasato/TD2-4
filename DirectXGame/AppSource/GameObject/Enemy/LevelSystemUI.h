#pragma once
#include <GameObject/Enemy/EnemyManager.h>
#include "LevelSystem.h"
#include <Render/RenderObject.h>
#include <Utility/Easing.h>
#include <vector>
#include <array>
#include <memory>

class LevelSystemUI {
public:
	~LevelSystemUI();

    void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(const LevelSystem& levelSystem, Matrix4x4 vpMatrix, float deltaTime);
    void Draw(CmdObj* cmdObj);
    void DrawImGui();

private:

    void Load();
	void Save();
    
    const int kSegment_ = 1000;
    std::vector<Vector2> vertices_;

	std::unique_ptr<SHEngine::RenderObject> render_;
    std::unique_ptr<SHEngine::RenderObject> currentPoint_;

	Transform transform_;
	Matrix4x4 wvpMatrix_;

	Transform cpTransform_;
	Matrix4x4 cpwvpMatrix_;
	int cpTextureIndex_ = 0;

	BinaryManager binaryManager_;
	std::string saveFilePath_ = "LevelSystemUI.bin";

	float frequency_ = 2.0f;
	float amplitude_ = 0.2f;
};
