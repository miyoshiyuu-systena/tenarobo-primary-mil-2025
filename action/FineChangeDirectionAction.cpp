#include "FineChangeDirectionAction.h"
#include "spikeapi.h"
#include <math.h>
#include "Logger.h"

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
                Logger::getInstance().logDebug("FineChangeDirectionAction: BLUE_BOTTLE_XY");
                break;
            default:
                return;
        }
        dly_tsk(1000 * 1000); // 1s待機して画像分析の更新を待つ

        uint16_t x = 0;
        uint16_t y = 0;
        int trial = 0;
        int x_diff = 0;
        int y_diff = 0;
        float angle = 0;

        /**
         * 本来はロボットビジョンとキャリブレーションで正確な位置推定ができる
         * 今回は簡易的なものにとどめる
         * 画像下部中央を起点とした三角推量で対象物までの水平角度を求める
         * 
         * 例
         * 画像のサイズ320×240
         * 得られた座標x=230, y=90の時（画像の右上らへん、ロボットの体の右前方遠く）
         * Tan-1((240 - 90) / (230 - 160))
         * ここで160というのは画像の横サイズの半分
         */

        /**
         * レベル1
         * やや大きめの角度変更 (20°)ほど
         */
        while (trial < 3) { // 検出精度が低く検知できないことがあるため、3回試行する
            Logger::getInstance().logDebug("FineChangeDirectionAction: trial: " + std::to_string(trial));
            coordinateCallback(x, y);
            Logger::getInstance().logDebug("FineChangeDirectionAction: x, y: " + std::to_string(x) + ", " + std::to_string(y));
            if (x != 0 && y != 0) {
                break;
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
        }
        
        Logger::getInstance().logDebug("x, y: " + std::to_string(x) + ", " + std::to_string(y));

        if (x != 0 && y != 0) {
            x_diff = (int)x - IMAGE_WIDTH / 2;
            y_diff = IMAGE_HEIGHT - (int)y;
            angle = atan2(y_diff, x_diff) * 180 / M_PI; // 何この関数
            if (angle > 20.0f) {
                device->twinWheelDrive.leftPivotTurn(90);
                dly_tsk((int)((angle / 90.0f) * 1000 * 1000));
                device->twinWheelDrive.stop();
            } else if (angle < -20.0f) {
                device->twinWheelDrive.rightPivotTurn(90);
                dly_tsk((int)((-angle / 90.0f) * 1000 * 1000));
                device->twinWheelDrive.stop();
            } else {
                // 角度が20°以内ならば、何もしない
            }
        } else {
            // 検出できなかったら諦める
        }

        trial = 0;

        /**
         * レベル2
         * 普通めの角度変更（10°）ほど
         */
         while (trial < 3) {
            coordinateCallback(x, y);
            if (x != 0 && y != 0) {
                break;
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
         }

         if (x != 0 && y != 0) {
            x_diff = (int)x - IMAGE_WIDTH / 2;
            y_diff = IMAGE_HEIGHT - (int)y;
            angle = atan2(y_diff, x_diff) * 180 / M_PI;
            if (angle > 10.0f) {
               device->twinWheelDrive.leftPivotTurn(45);
               dly_tsk((int)((angle / 45.0f) * 1000 * 1000));
               device->twinWheelDrive.stop();
            } else if (angle < -10.0f) {
               device->twinWheelDrive.rightPivotTurn(45);
               dly_tsk((int)((-angle / 45.0f) * 1000 * 1000));
               device->twinWheelDrive.stop();
            } else {
                // 角度が10°以内ならば、何もしない
            }
         } else {
            // 検出できなかったら諦める
         }

         trial = 0;

         /**
          * レベル3
          * 小さい角度変更（5°）ほど
          */
         while (trial < 3) {
            coordinateCallback(x, y);
            if (x != 0 && y != 0) {
                break;
            }
            trial++;
            dly_tsk(100 * 1000); // 100ms待機して画像分析の更新を待つ
         }

         if (x != 0 && y != 0) {
            x_diff = (int)x - IMAGE_WIDTH / 2;
            y_diff = IMAGE_HEIGHT - (int)y;
            angle = atan2(y_diff, x_diff) * 180 / M_PI;
            if (angle > 5.0f) {
               device->twinWheelDrive.leftPivotTurn(30);
               dly_tsk((int)((angle / 30.0f) * 1000 * 1000));
               device->twinWheelDrive.stop();
            } else if (angle < -5.0f) {
               device->twinWheelDrive.rightPivotTurn(30);
               dly_tsk((int)((-angle / 30.0f) * 1000 * 1000));
               device->twinWheelDrive.stop();
            } else {
               // 角度が5°以内ならば、何もしない
            }
         } else {
            // 検出できなかったら諦める
         }

         /**
          * レベル1-3の動きで大体目標物までの角度が5°以内に収まりそう
          */
    };
}   