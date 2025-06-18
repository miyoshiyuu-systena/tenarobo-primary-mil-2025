#include "logic/CalcErrorOnBlueWhiteBorder.h"

static const float BLUE_WHITE_BORDER_V_IDEAL = 60.0f;

/**
 * 青床と白床の境界線からの誤差を計算する
 * @return 青床と白床の境界線からの誤差(0-100)
 */
float calc_error_on_blue_white_border(int h, int s, int v)
{
    /**
     * XXX: この式は不適切である
     * 色相がラジアンであるため、実験的に青と白の境界値の意味付けを強化するべし
     */
    // return (float)(((BLUE_WHITE_BORDER_V_IDEAL - s) / 100.0f) + ((190.0f - h) / 255.0f));
    return (float)((BLUE_WHITE_BORDER_V_IDEAL - s) / 100.0f);
}