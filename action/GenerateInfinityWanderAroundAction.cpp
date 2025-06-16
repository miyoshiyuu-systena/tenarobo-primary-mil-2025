#include    "spikeapi.h"
#include    "action/GenerateInfinityWanderAroundAction.h"
#include    "share/ModuleAccess.h"
#include    "share/PerceptionDataAccess.h"
#include    "action/ActionChain.h"
#include    "action/RunUntilWallDetectAction.h"
#include    "action/Turn180Action.h"

/**
 * 壁を見つけるまで猪突猛進 ->
 * その場でくるりん
 * を繰り返す
 */
void generate_infinity_wander_around_action(ActionChain* next_ptr)
{
    ActionChain* runAction = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        run_until_wall_detect_action,
        "ペットボトルに激突するまで猪突猛進！！"
    );
    next_ptr = runAction;

    ActionChain* turnAction = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        turn_180_action,
        "その場でくるりん！！"
    );
    runAction->setNext(runAction);

    ActionChain* infinityAction = new ActionChain(
        &twinWheelDrive,
        &frontArm,
        perceptionDataAccess,
        generate_infinity_wander_around_action,
        "無限に生成する・・・"
    );
    runAction->setNext(infinityAction);
}