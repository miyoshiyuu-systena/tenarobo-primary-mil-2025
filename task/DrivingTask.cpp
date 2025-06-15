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
    file_logger->set_level(spdlog::level::info);
    file_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%L] [%n] %v");
    file_logger->info("アプリケーションを開始します。");
    file_logger->warn("設定ファイルが見つかりませんでした。デフォルト値を使用します。");
    file_logger->error("処理中にエラーが発生しました: {}", "無効な入力");
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