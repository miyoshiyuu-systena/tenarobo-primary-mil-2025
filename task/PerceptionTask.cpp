#include    "PerceptionTask.h"

//  知覚処理の実行周期(5s)
const int16_t perception_cycle_delay = 5000;

/**
 * 知覚処理
 * @param   exinf     拡張情報
 */
void    perception_task(intptr_t exinf)   {
    while(1) {
        /**
         * カメラデータの取得
         */
        //  TODO    miyoshi: カメラデータの取得

        /**
         * カラーセンサデータの取得
         */
        //  TODO    miyoshi: カラーセンサデータの取得

        /**
         * 超音波センサデータの取得
         */
        //  TODO    miyoshi: 超音波センサデータの取得

        /**
         * 待機
         */
        dly_tsk(perception_cycle_delay);
    }
    //  タスク終了
    ext_tsk();
}