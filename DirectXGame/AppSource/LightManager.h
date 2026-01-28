#pragma once
#include"Utility/Vector.h"

// 平行光源の構造体
struct DirectionalLight
{
    Vector4 color;
    Vector3 direction;
    float intensity;
    Vector3 cameraWorldPos;
};

class LightManager {
public:

    // 平行光源ライト
    static inline DirectionalLight light_;
};