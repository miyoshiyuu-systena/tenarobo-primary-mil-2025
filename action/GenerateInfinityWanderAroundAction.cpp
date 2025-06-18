#include    "spikeapi.h"
#include    "action/GenerateInfinityWanderAroundAction.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionNode.h"
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"

/**
 * 壁を見つけるまで猪突猛進 ->
 * その場でくるりん
 * を繰り返す
 */
std::function<void(ActionNode*&)> generate_infinity_wander_around_action()
{
    return [](ActionNode*& next_ptr) {
    ActionNode* runAction = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action(
            400,    // 壁の接近を検知する距離mm
            100,    // モーターの速度出力mm/s
            1000    // 壁の接近を検知する間隔時間ms
        ),
        "ペットボトルに激突するまで猪突猛進！！",
        0
    );
    next_ptr = runAction;

    ActionNode* turnAction = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action(1000),
        "その場でくるりん！！",
        0
    );
    runAction->setNext(turnAction);

    ActionNode* infinityAction = new ActionNode(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        generate_infinity_wander_around_action(),
        "無限に生成する・・・",
        0
    );
    turnAction->setNext(infinityAction);
    };
}