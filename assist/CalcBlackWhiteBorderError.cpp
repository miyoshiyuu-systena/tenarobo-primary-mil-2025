#include "CalcBlackWhiteBorderError.h"

/**
 * XXX: debug
 * 三好家の床では60の方がうまくいく
 */
// static const float BLACK_WHITE_BORDER_V_IDEAL = 45.0f;
static const float BLACK_WHITE_BORDER_V_IDEAL = 60.0f;

CalcErrorFunc calcBlackWhiteBorderError = [](int h, int s, int v) {
    return (float)((v - BLACK_WHITE_BORDER_V_IDEAL) / 100.0f);
};