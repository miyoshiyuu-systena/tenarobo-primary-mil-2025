#include "CalcBlueWhiteBorderError.h"
#include <cmath>

static const float BLUE_HUE_IDEAL = 240.0f; // 青色の理想的なH値
static const float BLUE_SATURATION_IDEAL = 60.0f; // 青色の理想的なS値

CalcErrorFunc calcBlueWhiteBorderError = [](int h, int s, int v) {
    // H値の角度差を計算（循環性を考慮）
    float hue_diff = std::abs(h - BLUE_HUE_IDEAL);
    if (hue_diff > 180.0f) {
        hue_diff = 360.0f - hue_diff; // 短い方の角度差を使用
    }
    float hue_error = hue_diff / 180.0f; // 最大誤差180度で標準化
    
    // S値の誤差を計算
    float saturation_error = std::abs(s - BLUE_SATURATION_IDEAL) / 100.0f;
    
    return (hue_error + saturation_error) / 2.0f;
};