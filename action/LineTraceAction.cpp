#include "spikeapi.h"
#include "LineTraceAction.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"
#include <functional>

using   namespace   spikeapi;


/**
 * 輝度の閾値
 * 白と黒の境目部分をロボットの左右中心が通過するように、両輪の速度を調整する
 * カラーセンサはロボットの左右中心鉛直下方向に取り付けられており、床面の輝度を取得できる
 * @note
 *      実験的に確定すること
 *      輝度の値は大会会場の光学環境と床の材料に影響を受ける
 * 
 * @note
 *      輝度とは
 *      面積のある光源を観測者側から見たときの眩しさを示す単位です。
 *      ディスプレイなどの光源や照明の明るさの指標として使われることが多く、輝度が高いほど明るくなり、輝度が低いほど輝度の値は低くなります。
 *      
 *      ライントレースにおける輝度の有効性
 *      黒い素材は光を吸収し、白い素材は光を反射する性質がある。
 *      すなわち、黒い素材は輝度が低くなり、白い素材は輝度が大きくなる。
 *      これを利用して、床面のグレースケールを判定する
 */
/**
 * HACK: 輝度を使用しないで済むかも
 * そもそも輝度を使用せずに、HSV値を使用して判定できる可能性あり。
 */
const float THRESHOLD_DEFAULT = 40.0f;

/**
 * 比例定数
 * @note
 *      比例定数を大きくすると
 *      ラインから外れかけたときに鋭く補正がかかる
 *      ただし、大きく方向が変わるなどの問題もあり得る
 */
const float Kp = 0.5f;

/**
 * ライントレースアクションを生成するファクトリー関数
 * @note
 *      この関数で作成したアクションでは、ロボットが黒いラインの右縁に沿って移動する
 */
std::function<void(ActionNode*&)> line_trace_action(float speed, int duration, std::function<bool()> judge)
{
    return [speed, duration, judge](ActionNode*& next_ptr) {
        do {
            /**
             * P制御による駆動指示
             * errorは、実測した輝度が閾値よりもどのくらい明るかったかを表す。
             * (負数のときは閾値よりも暗かったことを表している。)
             * 
             * errorが正数のときは目標よりも白いところを通っているため、右輪を加速して左方向に移動する
             * errorが負数のときは目標よりも黒いところを通っているため、左輪を加速して右方向に移動する
             * この細かい調整により、黒い線の右縁を左右中心に移動するようにロボットの動きを最適化している
             * 
             * @note 
             *      P制御とは
             *      制御対象の目標値と現在の値の偏差（誤差）に比例した操作量を出す制御方式
             *      今回の場合、比例係数Kpを使用している
             */
            const float error = perceptionDataAccess.color[2] - THRESHOLD_DEFAULT;
            twinWheelDrive.setSpeed(
                speed - Kp * error,     //  左輪
                speed + Kp * error      //  右輪
            );

            // 判定周期
            dly_tsk(duration * 1000);
        } while (!judge());

        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();
    };
}

/**
 * 青線上にいるかどうかを判定する
 * @return 青線上にいるかどうか
 */
bool is_on_blue_line(void)
{
    const int H_UPPER_THRESHOLD = 220;
    const int H_LOWER_THRESHOLD = 180;
    const int S_UPPER_THRESHOLD = 200;
    const int S_LOWER_THRESHOLD = 50;
    const int V_UPPER_THRESHOLD = 100;
    const int V_LOWER_THRESHOLD = 50;

    return (
        (H_LOWER_THRESHOLD <= perceptionDataAccess.color[0] && perceptionDataAccess.color[0] <= H_UPPER_THRESHOLD) &&
        (S_LOWER_THRESHOLD <= perceptionDataAccess.color[1] && perceptionDataAccess.color[1] <= S_UPPER_THRESHOLD) &&
        (V_LOWER_THRESHOLD <= perceptionDataAccess.color[2] && perceptionDataAccess.color[2] <= V_UPPER_THRESHOLD)
    );
}