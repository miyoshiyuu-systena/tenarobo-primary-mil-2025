#include "app.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoFrontAction.h"
#include "perception/Perception.h"

using namespace spikeapi;

/**
 * 知覚タスク
 * @param exinf 拡張情報
 */
void perc_task(intptr_t exinf) {
    perception.update();
}

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // カメラ初期化は知覚タスクで行うため、ここでは削除
    // 知覚タスクの開始
    sta_cyc(PERC_CYC);

    /**
     * ----------------------------
     * アクションチェーンの作成
     * ----------------------------
     */
    logger.logInfo("1000秒間待機します。その間に知覚情報を取得して！！");
    dly_tsk(1000 * 1000 * 1000);

    // 知覚タスクの停止
    stp_cyc(PERC_CYC);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}