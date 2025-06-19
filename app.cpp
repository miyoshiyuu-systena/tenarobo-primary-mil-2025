#include "app.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "action-chain/ActionNode.h"
#include "device/Device.h"
#include "action-chain/action/assist/LaneTracingAssist.h"
#include "action-chain/action/assist/CalcBlackWhiteBorderError.h"
#include "action-chain/action/assist/CalcBlueWhiteBorderError.h"
#include "action-chain/action/closer/BlueFloorCloser.h"
#include "action-chain/action/closer/BlackFloorCloser.h"
#include "action-chain/action/closer/TimedCloser.h"
#include "action-chain/action/HachikouAction.h"
#include "action-chain/action/GoStraightAction.h"
#include "action-chain/action/GoCurveAction.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}