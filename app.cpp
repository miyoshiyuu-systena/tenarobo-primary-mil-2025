#include "app.h"
#include <stdio.h>
#include "InitTask.h"
#include "PerceptionTask.h"
#include "DrivingTask.h"

//  メイン処理の実行周期(1s)
const int16_t main_cycle_delay = 1000;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    while(1) {
        /**
         * 通信部 -> 制御部データコピー
         */
        //  TODO    miyoshi: 通信部 -> 制御部データコピー

        /**
         * 意思決定
         */
        //  TODO    miyoshi: 意思決定

        /**
         * 制御部 -> 通信部データコピー
         */
        //  TODO    miyoshi: 制御部 -> 通信部データコピー

        /**
         * 待機
         */
        dly_tsk(main_cycle_delay);
    }
    //  タスク終了
    ext_tsk();
}