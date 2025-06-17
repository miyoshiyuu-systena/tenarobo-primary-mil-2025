#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
  
/* タスク優先度 */
#define     PERC_PRIORITY       (TMIN_APP_TPRI + 1)
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 2)
#define     CAMERA_PRIORITY     (TMIN_APP_TPRI + 3)  // カメラタスクは最低優先度

/* サイクル起動周期 */
#define     PERC_PERIOD         (10 * 1000)      // 10ms
#define     CAMERA_PERIOD       (200 * 1000)    // 200ms (5fps)

/* サイクル起動位相（最初に起動するときの遅延時間） */
#define     PERC_PHS            (0)
#define     CAMERA_PHS          (1 * 1000)    // 1ms遅延

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     TOPPERS_MACRO_ONLY

extern void perc_task(intptr_t exinf);
extern void perc_cyc(intptr_t exinf);
extern void main_task(intptr_t exinf);
extern void camera_task(intptr_t exinf);
extern void camera_cyc(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
