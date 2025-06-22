#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
  
/* タスク優先度 */
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 1)
#define     IMAGE_ACQ_PRIORITY  (TMIN_APP_TPRI + 2)  // 画像取得タスク（高優先度）

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void image_acquisition_task(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
