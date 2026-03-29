#include "LevelSystemUI.h"
#include <imgui/imgui.h>
#include <Utility/Easing.h> // Make sure Easing.h is included properly if not via header.

void LevelSystemUI::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager) {
    texNormal_ = textureManager->LoadTexture("EnemyInfo/Normal.png");
    texFast_ = textureManager->LoadTexture("EnemyInfo/Fast.png");
    texTackle_ = textureManager->LoadTexture("EnemyInfo/Tackle.png");
    texAll_ = textureManager->LoadTexture("EnemyInfo/All.png");

    int planeModelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");
    auto planeModelData = modelManager->GetNodeModelData(planeModelHandle);
    auto drawData = drawDataManager->GetDrawData(planeModelData.drawDataIndex);

    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            waveUIs_[i].renders[j] = std::make_unique<SHEngine::RenderObject>("WaveUI_" + std::to_string(i) + "_" + std::to_string(j));
            waveUIs_[i].renders[j]->Initialize();
            waveUIs_[i].renders[j]->SetDrawData(drawData);
            waveUIs_[i].renders[j]->psoConfig_.vs = "Simples.VS.hlsl";
            waveUIs_[i].renders[j]->psoConfig_.ps = "TexColors.PS.hlsl";
            waveUIs_[i].renders[j]->psoConfig_.isSwapChain = false;
            waveUIs_[i].renders[j]->CreateSRV(sizeof(Matrix4x4), 1, ShaderType::VERTEX_SHADER, "WVP");
            waveUIs_[i].renders[j]->CreateSRV(sizeof(Vector4), 1, ShaderType::PIXEL_SHADER, "Color");
            waveUIs_[i].renders[j]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
            waveUIs_[i].renders[j]->SetUseTexture(true);
            waveUIs_[i].renders[j]->instanceNum_ = 1;
        }
    }
    
    lastWaveIndex_ = -1;
    animState_ = AnimState::Idle;
    animTimer_ = 0.0f;
    currentWaveTypes_.clear();
    pendingWaveCount_ = 0;
}

void LevelSystemUI::Update(float deltaTime, int currentWave, const std::vector<std::vector<EnemyType>>& waveTypes, const Matrix4x4& orthoVpMatrix) {
    if (lastWaveIndex_ == -1) {
        lastWaveIndex_ = currentWave;
        currentWaveTypes_ = waveTypes;
    }

    if (currentWave > lastWaveIndex_) {
        pendingWaveCount_ += (currentWave - lastWaveIndex_);
        lastWaveIndex_ = currentWave;
        targetWaveTypes_ = waveTypes;
    }

    if (animState_ == AnimState::Idle && pendingWaveCount_ > 0) {
        animState_ = AnimState::ShrinkFirst;
        animTimer_ = 0.0f;
    }

    // Update pulse timer for leftmost UI
    pulseTimer_ += deltaTime;
    if (pulseTimer_ >= pulseDuration_) {
        pulseTimer_ = 0.0f;
    }

    float t = 0.0f;
    if (animState_ != AnimState::Idle) {
        animTimer_ += deltaTime;
        t = std::clamp(animTimer_ / animDuration_, 0.0f, 1.0f);
        
        if (animTimer_ >= animDuration_) {
            animTimer_ = 0.0f;
            if (animState_ == AnimState::ShrinkFirst) {
                animState_ = AnimState::MoveLeft;
                if (!currentWaveTypes_.empty()) {
                    currentWaveTypes_.erase(currentWaveTypes_.begin());
                }
            } else if (animState_ == AnimState::MoveLeft) {
                animState_ = AnimState::ExpandLast;
                if (!targetWaveTypes_.empty() && targetWaveTypes_.size() == 5) {
                    currentWaveTypes_.push_back(targetWaveTypes_[4]);
                } else if (!targetWaveTypes_.empty()) {
                    currentWaveTypes_.push_back(targetWaveTypes_.back());
                }
            } else if (animState_ == AnimState::ExpandLast) {
                animState_ = AnimState::Idle;
                pendingWaveCount_--;
                if (pendingWaveCount_ == 0) {
                    currentWaveTypes_ = waveTypes;
                }
            }
        }
    } else {
        currentWaveTypes_ = waveTypes;
    }

    float baseX = uiBaseX_;
    float baseY = uiBaseY_;

    // Reset loop
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 2; ++j) {
            waveUIs_[i].isActive[j] = false;
        }
    }

    for (int i = 0; i < currentWaveTypes_.size(); ++i) {
        if (i >= 5) break; // Guard bounds

        std::vector<EnemyType> types = currentWaveTypes_[i];
        waveUIs_[i].typeCount = static_cast<int>(types.size());

        if (types.empty()) continue;

        float waveX = baseX + i * uiSpacing_;
        float scale = 1.0f;

        if (animState_ == AnimState::ShrinkFirst) {
            if (i == 0) {
                scale = 1.0f - Apply(t, EaseType::EaseOutCubic);
            }
        } else if (animState_ == AnimState::MoveLeft) {
            // In MoveLeft phase, current wave types has 4 items which represents index 1 to 4 initially
            float moveOffset = Apply(t, EaseType::EaseInOutQuad) * uiSpacing_;
            waveX = baseX + (i + 1) * uiSpacing_ - moveOffset;
        } else if (animState_ == AnimState::ExpandLast) {
            if (i == 4) {
               scale = Apply(t, EaseType::EaseOutBack);
            }
        }

        // Apply pulsing effect to leftmost UI (i == 0)
        if (i == 0 && animState_ == AnimState::Idle) {
            float pulseT = pulseTimer_ / pulseDuration_;
            // Use EaseInOutQuad for smooth pulsing effect (0 -> 1 -> 0)
            float pulseProgress = pulseT < 0.5f 
                ? Apply(pulseT * 2.0f, EaseType::EaseOutQuad) 
                : Apply((1.0f - pulseT) * 2.0f, EaseType::EaseOutQuad);
            float pulseScale = 1.0f - pulseProgress * 0.15f; // Scale from 1.0 to 0.85
            scale *= pulseScale;
        }

        Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Matrix4x4 wvp;

        if (types.size() >= 3) {
            waveUIs_[i].isActive[0] = true;
            int texIndex = texAll_;

            Matrix4x4 world = Matrix::MakeAffineMatrix({64.0f * scale, 64.0f * scale, 1.0f}, Vector3(), {waveX, baseY, 0.0f});
            wvp = world * orthoVpMatrix;

            waveUIs_[i].renders[0]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
            waveUIs_[i].renders[0]->CopyBufferData(1, &color, sizeof(Vector4));
            waveUIs_[i].renders[0]->CopyBufferData(2, &texIndex, sizeof(int));
        } else if (types.size() == 2) {
            for (int j = 0; j < 2; ++j) {
                waveUIs_[i].isActive[j] = true;
                int texIndex = texNormal_;
                if (types[j] == EnemyType::Normal) texIndex = texNormal_;
                else if (types[j] == EnemyType::Fast) texIndex = texFast_;
                else if (types[j] == EnemyType::Tackle) texIndex = texTackle_;

                float shiftX = ((j == 0) ? -16.0f : 16.0f) * scale;
                Matrix4x4 world = Matrix::MakeAffineMatrix({32.0f * scale, 64.0f * scale, 1.0f}, Vector3(), {waveX + shiftX, baseY, 0.0f});
                wvp = world * orthoVpMatrix;

                waveUIs_[i].renders[j]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
                waveUIs_[i].renders[j]->CopyBufferData(1, &color, sizeof(Vector4));
                waveUIs_[i].renders[j]->CopyBufferData(2, &texIndex, sizeof(int));
            }
        } else if (types.size() == 1) {
            for (int j = 0; j < 2; ++j) {
                waveUIs_[i].isActive[j] = true;
                int texIndex = texNormal_;
                if (types[0] == EnemyType::Normal) texIndex = texNormal_;
                else if (types[0] == EnemyType::Fast) texIndex = texFast_;
                else if (types[0] == EnemyType::Tackle) texIndex = texTackle_;

                float shiftX = ((j == 0) ? -16.0f : 16.0f) * scale;
                Matrix4x4 world = Matrix::MakeAffineMatrix({32.0f * scale, 64.0f * scale, 1.0f}, Vector3(), {waveX + shiftX, baseY, 0.0f});
                wvp = world * orthoVpMatrix;

                waveUIs_[i].renders[j]->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
                waveUIs_[i].renders[j]->CopyBufferData(1, &color, sizeof(Vector4));
                waveUIs_[i].renders[j]->CopyBufferData(2, &texIndex, sizeof(int));
            }
        }
    }
}

void LevelSystemUI::Draw(CmdObj* cmdObj) {
    for (auto& waveUI : waveUIs_) {
        for (int j = 0; j < 2; ++j) {
            if (waveUI.renders[j] && waveUI.isActive[j]) {
                waveUI.renders[j]->Draw(cmdObj);
            }
        }
    }
}

void LevelSystemUI::DrawImGui() {
#ifdef USE_IMGUI
    ImGui::Begin("Level System UI");
    ImGui::Text("Wave UI Settings");
    ImGui::DragFloat("UI Base X", &uiBaseX_, 1.0f);
    ImGui::DragFloat("UI Base Y", &uiBaseY_, 1.0f);
    ImGui::DragFloat("UI Spacing", &uiSpacing_, 1.0f);
    ImGui::End();
#endif
}