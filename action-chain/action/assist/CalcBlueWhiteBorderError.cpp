#include "CalcBlackWhiteBorderError.h"
#include "CalcErrorFunc.h"

CalcErrorFunc calcBlueWhiteBorderError = [](int h, int s, int v) {
    return (float)((((60.0f - s) / 100.0f) + ((190.0f - h) / 360.0f)));
};