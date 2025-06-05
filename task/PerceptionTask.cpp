#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"

/**
 * 知覚処理
 * @param   exinf     拡張情報
 */
void    perc_task(intptr_t exinf)   {
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
    syslog(LOG_WARNING, "Perception_Task is running");

    //  タスク終了
    ext_tsk();
}