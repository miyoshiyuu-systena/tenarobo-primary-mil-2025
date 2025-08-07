#include "CalcBlackWhiteBorderError.h"
#include "config.h"

/**
 * 設定ファイルから白黒境界のV値理想値を取得する関数
 */
static float getBlackWhiteBorderVIdeal() {
    // デフォルト値は60.0f（三好家の床での最適値）
    return config.getFloatValue("blackWhiteBorderVIdeal", 60.0f);
}

CalcErrorFunc calcBlackWhiteBorderError = [](int h, int s, int v) {
    static float BLACK_WHITE_BORDER_V_IDEAL = getBlackWhiteBorderVIdeal();
    return (float)((v - BLACK_WHITE_BORDER_V_IDEAL) / 100.0f);
};