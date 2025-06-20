#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
#include    "web-camera/CameraTask.h"
  
/* タスク優先度 */
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 1)
#define     CAMERA_PRIORITY     (TMIN_APP_TPRI + 2)

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     CAMERA_PERIOD
#define     CAMERA_PERIOD       (50 * 1000)     // 50ms
#endif      /* CAMERA_PERIOD */

#ifndef     CAMERA_PHS
#define     CAMERA_PHS          (TA_NULL)
#endif      /* CAMERA_PHS */

#ifndef     TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void camera_task(intptr_t exinf);
extern void camera_cyc(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
