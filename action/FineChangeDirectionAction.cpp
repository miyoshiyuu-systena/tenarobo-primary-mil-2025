#include "FineChangeDirectionAction.h"
#include "spikeapi.h"
#include <math.h>

ActionCall fineChangeDirectionActionFactory(
    enum AnalysisCommand command
)
{
    return [
        command
    ](
        ActionNode*& curr_ptr,
        ActionNode*& next_ptr,
        Device*& device
    ) {
        /**
         * 分析コマンドに応じて、座標を取得するコールバック関数を設定する
         * XXX リクエストとレスポンスをうまく組み合わせることができておらずswitchを使うハメに
         * XXX 要リファクタリング（どっちかというとImageAnalysisServerの方を工夫した方がいいかも）
         */
        CoordinateCallback coordinateCallback;
        switch (command) {
            case BLUE_BOTTLE_XY:
                ImageAnalysisServer::getInstance().request(command);
                coordinateCallback = ImageAnalysisServer::responseBlueBottleXYStatic;
                break;
            default:
                return;
        }
        dly_tsk(1000 * 1000); // 1s待機して画像分析の更新を待つ

        uint16_t x = 0;
        uint16_t y = 0;
        int x_diff = 0;
        int trial;

        trial = 0;

        /**
         * 大きめの位置調整
         * 大胆に動かす
         */
        while (trial < 10) {
            coordinateCallback(x, y);
            if (x != 0 && y != 0) {
                x_diff = (int)x - (int)IMAGE_WIDTH / 2;
                if (x_diff > 90) {
                    device->twinWheelDrive.leftPivotTurn(45);
                } else if (x_diff < -90) {
                    device->twinWheelDrive.rightPivotTurn(45);
                } else {
                    /* do nothing */
                }
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
            device->twinWheelDrive.stop();
            dly_tsk(50 * 1000); // 100ms待機して画像分析の更新を待つ
        }

        trial = 0;

        /**
         * 中頃の位置調整
         * 中位のスピードで動かす
         */
        while (trial < 10) {
            coordinateCallback(x, y);
            if (x != 0 && y != 0) {
                x_diff = (int)x - (int)IMAGE_WIDTH / 2;
                if (x_diff > 45) {
                    device->twinWheelDrive.leftPivotTurn(30);
                } else if (x_diff < -45) {
                    device->twinWheelDrive.rightPivotTurn(30);
                } else {
                    /* do nothing */
                }
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
            device->twinWheelDrive.stop();
            dly_tsk(50 * 1000); // 100ms待機して画像分析の更新を待つ
        }

        trial = 0;

        /**
         * 小さめの位置調整
         * 小さいスピードで動かす
         */
        while (trial < 10) {
            coordinateCallback(x, y);
            if (x != 0 && y != 0) {
                x_diff = (int)x - (int)IMAGE_WIDTH / 2;
                if (x_diff > 20) {
                    device->twinWheelDrive.leftPivotTurn(15);
                } else if (x_diff < -20) {
                    device->twinWheelDrive.rightPivotTurn(15);
                } else {
                    /* do nothing */
                }
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
            device->twinWheelDrive.stop();
            dly_tsk(50 * 1000); // 100ms待機して画像分析の更新を待つ
        }

        ImageAnalysisServer::getInstance().request(DO_NOTHING);
    };
}   