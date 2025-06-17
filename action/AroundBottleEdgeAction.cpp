#include    "AroundBottleEdgeAction.h"
#include    "spikeapi.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"

/**
 * ペットボトルの周りを5秒かけて半円回る
 */
void aroundBottleEdgeAction(ActionNode*& next_ptr)
{
    //右曲がりに回る(5秒で半径100ミリメートルの円を半周する)
    twinWheelDrive.curveRightSpeed(62.832, 100);

    // TOPPERS/ASP3インターフェースの待機関数（マイクロ秒）
    // 5秒待機
    dly_tsk(5 * 1000 * 1000);

    // モーターを停止
    twinWheelDrive.stop();
}