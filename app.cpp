#include    "app.h"
#include    <stdio.h>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "DrivingTask.h"
#include    "logging/Logger.h"

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    logger.logInfo("=== ロボット制御システム開始 ===");
    logger.logInfo("システム初期化を開始");

    /**
     * センサーデータ取得インスタンスの生成
     */
    //  TODO miyoshi: センサーの初期化
    logger.logInfo("センサーシステムの初期化中...");
    // 実際の初期化処理が追加される際のログ例
    // logger.logDebug("カラーセンサー初期化完了");
    // logger.logDebug("超音波センサー初期化完了");
    // logger.logDebug("ジャイロセンサー初期化完了");
    logger.logInfo("センサーシステム初期化完了");

    /**
     * シナリオチェーンの構成
     */
    //  TODO miyoshi: シナリオチェーンの構成
    logger.logInfo("シナリオチェーンの構成中...");
    // 実際の構成処理が追加される際のログ例
    // logger.logDebug("ライントレースシナリオ登録");
    // logger.logDebug("障害物回避シナリオ登録");
    // logger.logDebug("ゴール検出シナリオ登録");
    logger.logInfo("シナリオチェーン構成完了");

    /**
     * 駆動系インスタンスの生成
     */
    //  TODO miyoshi: 駆動系インスタンスの生成
    logger.logInfo("駆動系システムの初期化中...");
    // 実際の初期化処理が追加される際のログ例
    // logger.logDebug("左モーター初期化完了");
    // logger.logDebug("右モーター初期化完了");
    // logger.logDebug("アーム機構初期化完了");
    logger.logInfo("駆動系システム初期化完了");

    syslog(LOG_NOTICE, "INIT FINISH - Its start!!!");
    logger.logInfo("=== システム初期化完了 - 制御開始 ===");

    sta_cyc(PERC_CYC);
    sta_cyc(DRIV_CYC);
    logger.logInfo("周期タスク開始: PERC_CYC, DRIV_CYC");

    // 初期化完了時にログファイルを一度書き込み
    logger.writeLogsToFile();

    int loopCount = 0;
    int lastLogTime = 0;
    const int LOG_INTERVAL = 1000; // 1000回ループごとにログ出力
    
    logger.logInfo("メインループ開始");
    
    while(
        (1) &&  /* 停止ボタン（圧力センサ）が押されていない */  //TODO: 仮に無限ループとする  
        (1)     /* 全てのタスクが終了した or 異常終了した */    //TODO: 仮に無限ループとする
    ) {
        loopCount++;
        
        // 定期的な状態ログ
        if (loopCount % LOG_INTERVAL == 0) {
            logger.logInfo("メインループ実行中 - 総実行回数: " + std::to_string(loopCount));
        }

        /**
         * 通信部 -> 制御部データコピー
         */
        //  TODO    miyoshi: 通信部 -> 制御部データコピー
        if (loopCount % 2000 == 0) {
            logger.logDebug("通信部 -> 制御部データコピー実行");
            // 実際の通信処理が追加される際のログ例
            // logger.logDebug("センサーデータ受信: カラー値=" + std::to_string(colorValue));
            // logger.logDebug("センサーデータ受信: 距離=" + std::to_string(distance));
        }

        /**
         * 意思決定
         */
        //  TODO    miyoshi: 意思決定
        if (loopCount % 1500 == 0) {
            logger.logDebug("意思決定処理実行");
            // 実際の意思決定処理が追加される際のログ例
            // logger.logDebug("現在の状態: ライントレース中");
            // logger.logInfo("障害物検出 - 回避行動開始");
            // logger.logWarning("コース逸脱検出 - 補正動作実行");
        }

        /**
         * 制御部 -> 通信部データコピー
         */
        //  TODO    miyoshi: 制御部 -> 通信部データコピー
        if (loopCount % 2000 == 0) {
            logger.logDebug("制御部 -> 通信部データコピー実行");
            // 実際の制御命令送信が追加される際のログ例
            // logger.logDebug("モーター制御命令送信: 左=" + std::to_string(leftPower) + "%, 右=" + std::to_string(rightPower) + "%");
        }

        // エラーシミュレーション（テスト用）
        if (loopCount == 5000) {
            logger.logWarning("【テスト】警告レベルのイベントが発生しました");
        }
        
        if (loopCount == 7500) {
            logger.logError("【テスト】エラーレベルのイベントが発生しました（システムは継続動作中）");
        }

        // 定期的にログファイルに書き込み（5000回ループごと）
        if (loopCount % 5000 == 0) {
            logger.writeLogsToFile();
        }

        /**
         * 待機
         */
        // syslog(LOG_NOTICE, "Main_Task is running");
        
        // 例：10000回ループしたら終了（実際の実装では適切な終了条件を設定してください）
        if (loopCount >= 10000) {
            logger.logInfo("テスト用ループ回数上限に達しました");
            logger.logInfo("プログラム終了処理を開始");
            break;
        }
    }
    
    logger.logInfo("メインループ終了");
    
    stp_cyc(PERC_CYC);
    stp_cyc(DRIV_CYC);
    logger.logInfo("周期タスク停止完了");
    
    logger.logInfo("=== ロボット制御システム正常終了 ===");
    logger.logInfo("総実行ループ回数: " + std::to_string(loopCount));
    
    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    
    //  タスク終了
    ext_tsk();
}