#pragma once
#include <GameObject/Enemy/EnemyManager.h>
#include <SHEngine.h>
#include <Render/RenderObject.h>
#include <Utility/Easing.h>
#include <vector>
#include <array>
#include <memory>
class LevelSystemUI {
public:
    void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
    void Update(float deltaTime, int currentWave, const std::vector<std::vector<EnemyType>>& waveTypes, const Matrix4x4& orthoVpMatrix);
    void Draw(CmdObj* cmdObj);
    void DrawImGui();

private:
    struct WaveUIBlock {
        std::unique_ptr<SHEngine::RenderObject> renders[2];
        bool isActive[2] = {false, false};
        int typeCount = 0;
    };

    std::array<WaveUIBlock, 5> waveUIs_;
    int texNormal_ = 0;
    int texFast_ = 0;
    int texTackle_ = 0;
    int texAll_ = 0;

    float uiBaseX_ = 525.0f;
    float uiBaseY_ = -130.0f;
    float uiSpacing_ = 100.0f;

    // Animation states
    enum class AnimState {
        Idle,
        ShrinkFirst,
        MoveLeft,
        ExpandLast
    };

    AnimState animState_ = AnimState::Idle;
    float animTimer_ = 0.0f;
    float animDuration_ = 0.5f;

    int lastWaveIndex_ = -1;
    std::vector<std::vector<EnemyType>> currentWaveTypes_;
    int pendingWaveCount_ = 0; // Number of wave shifts pending
    std::vector<std::vector<EnemyType>> targetWaveTypes_; // For expanding the new wave safely
};
