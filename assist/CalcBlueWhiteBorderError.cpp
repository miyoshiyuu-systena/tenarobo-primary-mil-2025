#include "CalcBlueWhiteBorderError.h"
#include "config.h"
#include <cmath>

/**
 * 設定ファイルから青色の理想的なS値を取得する関数
 */
static float getBlueSaturationIdeal() {
    // 2025.07.29 試走会での測定
    // 青 h 214 s 97 v 64
    // 白 h 180 s 17 v 100
    // sは判定に使いやすいと考えられる
    // hは不安定だった
    return config.getFloatValue("blueSaturationIdeal", 55.0f);
}

CalcErrorFunc calcBlueWhiteBorderError = [](int h, int s, int v) {
    static float BLUE_SATURATION_IDEAL = getBlueSaturationIdeal();
    return (s - BLUE_SATURATION_IDEAL) / 100.0f;
};