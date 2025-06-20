#include "CalcBlueWhiteBorderError.h"
#include "config.h"
#include <cmath>

/**
 * 設定ファイルから青色の理想的なH値を取得する関数
 */
static float getBlueHueIdeal() {
    // デフォルト値は240.0f（青色の理想的なH値）
    return config.getFloatValue("blueHueIdeal", 240.0f);
}

/**
 * 設定ファイルから青色の理想的なS値を取得する関数
 */
static float getBlueSaturationIdeal() {
    // デフォルト値は60.0f（青色の理想的なS値）
    return config.getFloatValue("blueSaturationIdeal", 60.0f);
}

CalcErrorFunc calcBlueWhiteBorderError = [](int h, int s, int v) {
    static float BLUE_HUE_IDEAL = getBlueHueIdeal();
    static float BLUE_SATURATION_IDEAL = getBlueSaturationIdeal();
    
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