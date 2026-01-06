#pragma once
#include <fstream>
#include <sstream>
#include <windows.h>
#include <timeapi.h>
#include <Tool/Logger/Logger.h>

enum class FPSType {
    CPU,
    GPU,
    Count
};

class FPSObserver {
public:

    /// <summary>
    /// fpsを管理するクラス
    /// </summary>
    /// <param name="isFix">fpsを固定するかどうか</param>
    /// <param name="targetFps">固定するときの目標fps</param>
    FPSObserver(bool isFix = true, double targetFps = 60.0);
    ~FPSObserver();

    void SetIsFix(bool isFix, FPSType type = FPSType::CPU) { isFix_[int(type)] = isFix; };
    void SetTargetFPS(double targetfps, FPSType type = FPSType::CPU);
    float GetDeltatime(FPSType type = FPSType::CPU) { return deltatime_[int(type)]; }

    // FPSを固定するため処理時間に余裕がある場合その分待つ
    void TimeAdjustment(FPSType type = FPSType::CPU);

private:
    static std::vector<float> deltatime_;

    std::vector<double> targetFPS_;
    std::vector<double> frameTime_;

    std::vector<bool> isFix_;

    // 計測開始時間
    std::vector<LARGE_INTEGER> timeStart_{};
    // 計測終了時間  
    std::vector<LARGE_INTEGER> timeEnd_{};
    // 計測周波数
    std::vector<LARGE_INTEGER> timeFreq_{};

    // より精密な時間待機
    void PreciseSleep(double seconds, FPSType type);

    std::shared_ptr<spdlog::logger> logger_;
	uint32_t frameCount_ = 0;
};
