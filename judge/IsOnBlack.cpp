#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"

static const int H_UPPER_THRESHOLD = 255;
static const int H_LOWER_THRESHOLD = 0;
static const int S_UPPER_THRESHOLD = 255;
static const int S_LOWER_THRESHOLD = 0;
static const int V_UPPER_THRESHOLD = 30;
static const int V_LOWER_THRESHOLD = 0;

/**
 * 黒床上にいるかどうかを判定する
 * @return 黒床上にいるかどうか
 */
 bool is_on_black(void)
 {
     return (
         (H_LOWER_THRESHOLD <= perceptionDataAccess.color[0] && perceptionDataAccess.color[0] <= H_UPPER_THRESHOLD) &&
         (S_LOWER_THRESHOLD <= perceptionDataAccess.color[1] && perceptionDataAccess.color[1] <= S_UPPER_THRESHOLD) &&
         (V_LOWER_THRESHOLD <= perceptionDataAccess.color[2] && perceptionDataAccess.color[2] <= V_UPPER_THRESHOLD)
     );
 }