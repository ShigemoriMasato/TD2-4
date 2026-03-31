#include "LevelSystemUI.h"
#include <imgui/imgui.h>
#include <Utility/Easing.h> // Make sure Easing.h is included properly if not via header.

void LevelSystemUI::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
    
}

void LevelSystemUI::Update(const LevelSystem& levelSystem) {
}

void LevelSystemUI::Draw(CmdObj* cmdObj) {
    
}

void LevelSystemUI::DrawImGui() {
#ifdef USE_IMGUI
    
#endif
}