#include "FineChangeDirectionLineAction.h"
#include "ImageAnalysisServer.h"
#include "spikeapi.h"

ActionCall fineChangeDirectionLineActionFactory(bool isRight)
{
    return [isRight](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        device->twinWheelDrive.stop();

        int frontStraightPoint;
        ImageAnalysisServer::getInstance().request(FRONT_STRAIGHT);
        dly_tsk(500 * 1000); // 500ms待機して画像分析の更新を待つ

        while (true) {
            frontStraightPoint = 0;
            for (int i = 0; i < 4; i++) {
                frontStraightPoint = frontStraightPoint + (int)(ImageAnalysisServer::getInstance().responseFrontStraight());
                dly_tsk(60 * 1000); // 60ms待機して画像分析の更新を待つ
            }
            // 4回中3回直線検知したら十分とする
            if (frontStraightPoint >= 3) {
                break;
            }

            if (isRight) {
                device->twinWheelDrive.leftPivotTurn(30);
            } else {
                device->twinWheelDrive.rightPivotTurn(30);
            }
            dly_tsk(50 * 1000); // ちょっとだけ回転する
        }

        device->twinWheelDrive.stop();
        ImageAnalysisServer::getInstance().request(DO_NOTHING);
    };
}