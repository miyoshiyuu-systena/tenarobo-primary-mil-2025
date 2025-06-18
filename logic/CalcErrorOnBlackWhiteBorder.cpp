#include "logic/CalcErrorOnBlackWhiteBorder.h"

/**
 * XXX: debug
 * 三好家の床では60の方がうまくいく
 */
// static const float BLACK_WHITE_BORDER_V_IDEAL = 45.0f;
static const float BLACK_WHITE_BORDER_V_IDEAL = 60.0f;

/**
 * 黒床と白床の境界線からの誤差を計算する
 * @return 黒床と白床の境界線からの誤差(0-100)
 */
 float calc_error_on_black_white_border(int h, int s, int v)
 {
     return (float)((v - BLACK_WHITE_BORDER_V_IDEAL) / 100.0f);
 }
 