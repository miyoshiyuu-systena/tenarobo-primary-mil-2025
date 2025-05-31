#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
  
/* タスク優先度 */
#define     PERC_PRIORITY       (TMIN_APP_TPRI + 1)
#define     DRIV_PRIORITY       (TMIN_APP_TPRI + 2)
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 3)

/* サイクル起動周期 */
#define     PERC_PERIOD         (1000 * 1000)       //TODO: 仮に、分かりやすさのために1sとする
#define     DRIV_PERIOD         (4000 * 1000)       //TODO: 仮に、分かりやすさのために4sとする

/* サイクル起動位相（最初に起動するときの遅延時間） */
#define     PERC_PHS            (200 * 1000)        //TODO: 仮値
#define     DRIV_PHS            (600 * 1000)        //TODO: 仮値

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     TOPPERS_MACRO_ONLY

extern void perception_task(intptr_t exinf);
extern void driving_task(intptr_t exinf);
extern void main_task(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
