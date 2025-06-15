#include    <spdlog/spdlog.h>
#include    <spdlog/sinks/basic_file_sink.h>
#include    "syssvc/syslog.h"
#include    "DrivingTask.h"

/**
 * 駆動指示
 * @param   exinf     拡張情報
 */
void    driv_task(intptr_t exinf)   {
    auto file_logger = spdlog::basic_logger_mt("my_app_logger", "logs/application.log", true);
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