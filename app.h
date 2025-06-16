#ifdef __cplusplus
extern "C" {
#endif

#include    "spikeapi.h"
  
/* タスク優先度 */
#define     PERC_PRIORITY       (TMIN_APP_TPRI + 1)
#define     MAIN_PRIORITY       (TMIN_APP_TPRI + 2)

/* サイクル起動周期 */
#define     PERC_PERIOD         (100 * 1000)       //TODO: 仮に、分かりやすさのために100msとする

/* サイクル起動位相（最初に起動するときの遅延時間） */
#define     PERC_PHS            (0)

#ifndef     STACK_SIZE
#define     STACK_SIZE          (4096)
#endif      /* STACK_SIZE */

#ifndef     TOPPERS_MACRO_ONLY

extern void perc_task(intptr_t exinf);
extern void perc_cyc(intptr_t exinf);
extern void main_task(intptr_t exinf);

#endif      /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
