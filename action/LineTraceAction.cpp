#include "spikeapi.h"
#include "LineTraceAction.h"
#include "share/ModuleAccess.h"
#include "share/PerceptionDataAccess.h"
#include "action/ActionNode.h"
#include "logging/Logger.h"
#include <functional>

using   namespace   spikeapi;


/**
 * XXX: 実験的に決定してください。
 * ライントレースの白と黒の境目の値とします。
 * H / Vの値を使用します。
 */
const float THRESHOLD = 0.5f;

/**
 * XXX: 実験的に決定してください。
 * 比例定数
 */
const float Kp = 5.0f;

std::function<void(ActionNode*&)> line_trace_action(float speed, int duration, int vacation_duration, std::function<bool()> judge)
{
    return [speed, duration, vacation_duration, judge](ActionNode*& next_ptr) {
        Logger& logger = Logger::getInstance();
        do {
            // P制御による速度制御
            const float error = perceptionDataAccess.color[0] / perceptionDataAccess.color[2] - THRESHOLD;
            if (error > 0) {
                /**
                 * 白い床を検出：テープから離れすぎているので左に曲がる
                 */
                twinWheelDrive.setSpeed(speed + Kp * error, speed - Kp * error);
                logger.logInfo("白い床検出: 左に曲がる（テープに近づく）, 制御強度: " + std::to_string(Kp * error));
            } else {
                /**
                 * 黒いテープを検出：テープの上に乗りすぎているので右に曲がる
                 */
                twinWheelDrive.setSpeed(speed - Kp * error, speed + Kp * error);
                logger.logInfo("黒いテープ検出: 右に曲がる（白い床に近づく）, 制御強度: " + std::to_string(Kp * error));
            }

            // 判定周期
            dly_tsk(duration * 1000);
        } while (!judge());

        twinWheelDrive.stop();
        twinWheelDrive.resetLeftCount();
        twinWheelDrive.resetRightCount();   

        // 休憩
        dly_tsk(vacation_duration * 1000);
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