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
    void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
    
    void Draw(CmdObj* cmdObj);
    void DrawImGui();

private:
    
    std::vector<Vector2> levelIndicators_;

};
