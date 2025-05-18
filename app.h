#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
#include    "InitTask.h"
  
/* タスク優先度 */
#define     INIT_PRIORITY       (TMIN_APP_TPRI + 1)
#define     PERC_PRIORITY       (TMIN_APP_TPRI + 2)
#define     DRIV_PRIORITY       (TMIN_APP_TPRI + 3)
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 4)

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
