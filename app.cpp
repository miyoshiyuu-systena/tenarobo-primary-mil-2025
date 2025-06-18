#include    "app.h"
#include    "spikeapi.h"
#include    "logging/Logger.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // カメラ初期化は知覚タスクで行うため、ここでは削除
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    // アクションチェーンの処理
    dly_tsk(1000 * 1000 * 1000);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}