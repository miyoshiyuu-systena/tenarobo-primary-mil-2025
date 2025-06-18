#include "TwinWheelLogic.h"
#include "TwinWheelConst.h"
#include <cmath>

void calcStraightSpeed(float speed, float speeds[2])
{
    speeds[0] = speed;
    speeds[1] = speed;
}

void calculateCurveSpeeds(float angular_speed, bool is_right, float radius, float speeds[2])
{
    // 半径が車輪間隔の半分より小さい場合は停止
    if (radius < WHEEL_TREAD_MM / 2.0f) {
        speeds[0] = 0.0f;
        speeds[1] = 0.0f;
        return;
    }
    
    // 左曲線：右輪が外輪、左輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    // 角速度(°/s)から線速度(mm/s)に変換
    float outer_speed_mms = angular_speed * outer_radius * (M_PI / 180.0f);
    float inner_speed_mms = angular_speed * inner_radius * (M_PI / 180.0f);
    
    // mm/sから°/sに変換
    float outer_speed = outer_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    float inner_speed = inner_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0.0f) {
        inner_speed = 0.0f;
    }

    if (is_right) {
        speeds[0] = inner_speed;
        speeds[1] = outer_speed;
    } else {
        speeds[0] = outer_speed;
        speeds[1] = inner_speed;
    }
}