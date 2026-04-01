#include "DamagePopup.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>

void DamagePopup::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
    modelManager_ = modelManager;
    drawDataManager_ = drawDataManager;

    int planeModelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");
    auto planeModelData = modelManager->GetNodeModelData(planeModelHandle);
    SHEngine::DrawData data = drawDataManager->GetDrawData(planeModelData.drawDataIndex);

    text_ = std::make_unique<SHEngine::Text>();
    text_->Initialize(data, "YDWbananaslipplus.otf", 64);
    text_->SetSize(0.5f);
    text_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

    planeDrawInfo_.modelIndex = planeModelHandle;
    planeDrawInfo_.color = 0xffffffff;
    planeDrawInfo_.scale = { 1.0f, 1.0f, 1.0f };
}

void DamagePopup::Show(int damage, const Vector3& position) {
    position_ = position;
    position_.y += 1.5f;
    lifetime_ = 0.0f;
    isActive_ = true;

	rotate_ += Vector3(0.1f, 0.1f, 0.1f); // 回転の初期化（必要に応じて変更）

    text_->SetText(std::to_wstring(damage));
}

void DamagePopup::Update(float deltaTime, Matrix4x4 vpMatrix) {
    vpMatrix_ = vpMatrix;

    if (!isActive_) {
        return;
    }

    lifetime_ += deltaTime;
    if (lifetime_ >= maxLifetime_) {
        isActive_ = false;
        return;
    }

    position_.y += deltaTime * 1.5f;

    float alpha = 1.0f - (lifetime_ / maxLifetime_);
    text_->SetColor({ 1.0f, 1.0f, 1.0f, alpha });

    Transform transform;
    transform.position = position_;
    transform.scale = { 1.0f, 1.0f, 1.0f };
    transform.rotate = rotate_;
    text_->SetTransform(transform);
    text_->Update(vpMatrix_);
}

std::vector<DrawInfo> DamagePopup::GetDrawInfo() {
    std::vector<DrawInfo> infos;
    return infos;
}

void DamagePopup::Draw(CmdObj* cmdObj) {
    if (isActive_ && text_) {
        text_->Draw(cmdObj);
    }
}
