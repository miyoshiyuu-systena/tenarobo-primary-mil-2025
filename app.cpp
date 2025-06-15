#include    "app.h"
// #include    <stdio.h>
// #include    "PerceptionTask.h"
// #include    "DrivingTask.h"
#include    <spdlog/spdlog.h>
#include    <spdlog/sinks/basic_file_sink.h>
// #include    "syssvc/syslog.h"

#include    "spikeapi.h"

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    try {
        auto file_logger = spdlog::basic_logger_mt("my_app_logger", "logs/application.log", false);
        file_logger->set_level(spdlog::level::info);
        file_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%L] [%n] %v");
        file_logger->info("アプリケーションを開始します。");
        file_logger->warn("設定ファイルが見つかりませんでした。デフォルト値を使用します。");
        file_logger->error("処理中にエラーが発生しました: {}", "無効な入力");
        spdlog::shutdown();
    } catch (const spdlog::spdlog_ex& ex) {
        // syslog(LOG_ERROR, "ファイル生成時に例外が発生しています。");
    }
    /**
     * センサーデータ取得インスタンスの生成
     */
    //  TODO miyoshi: センサーの初期化

    /**
     * シナリオチェーンの構成
     */
    //  TODO miyoshi: シナリオチェーンの構成

    /**
     * 駆動系インスタンスの生成
     */
    //  TODO miyoshi: 駆動系インスタンスの生成

    // syslog(LOG_NOTICE, "INIT FINISH - Its start!!!");

    sta_cyc(PERC_CYC);
    sta_cyc(DRIV_CYC);

    while(
        (1) &&  /* 停止ボタン（圧力センサ）が押されていない */  //TODO: 仮に無限ループとする  
        (1)     /* 全てのタスクが終了した or 異常終了した */    //TODO: 仮に無限ループとする
    ) {
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
        // syslog(LOG_NOTICE, "Main_Task is running");
    }
    
    stp_cyc(PERC_CYC);
    stp_cyc(DRIV_CYC);
    
    //  タスク終了
    ext_tsk();
}