#include    "syssvc/syslog.h"
#include    "DrivingTask.h"

/**
 * 駆動指示
 * @param   exinf     拡張情報
 */
void    driving_task(intptr_t exinf)   {
    /**
     * 右モーターへの指示
     */
    //  TODO miyoshi: 右モーターへの指示

    /**
     * 左モーターへの指示
     */
    //  TODO miyoshi: 左モーターへの指示

    /**
     * アームモータへの指示
     */
    //  TODO miyoshi: アームモータへの指示

    /**
     * 待機
     */
    syslog(LOG_ERROR, "Driving_Task is running");

    //  タスク終了
    ext_tsk();
}