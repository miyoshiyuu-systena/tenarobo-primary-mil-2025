#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"

static const int H_UPPER_THRESHOLD = 220;
static const int H_LOWER_THRESHOLD = 180;
static const int S_UPPER_THRESHOLD = 200;
static const int S_LOWER_THRESHOLD = 50;
static const int V_UPPER_THRESHOLD = 100;
static const int V_LOWER_THRESHOLD = 50;

/**
 * 青床上にいるかどうかを判定する
 * @return 青床上にいるかどうか
 */
 bool is_on_blue(void)
 { 
     return (
         (H_LOWER_THRESHOLD <= perceptionDataAccess.color[0] && perceptionDataAccess.color[0] <= H_UPPER_THRESHOLD) &&
         (S_LOWER_THRESHOLD <= perceptionDataAccess.color[1] && perceptionDataAccess.color[1] <= S_UPPER_THRESHOLD) &&
         (V_LOWER_THRESHOLD <= perceptionDataAccess.color[2] && perceptionDataAccess.color[2] <= V_UPPER_THRESHOLD)
     );
 }