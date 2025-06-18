#ifndef TWIN_WHEEL_LOGIC_H
#define TWIN_WHEEL_LOGIC_H

/**
 * 左右の輪の速度を計算する
 * @param speed 左右の輪の速度[mm/s]
 * @param speeds 速度(mm/s)を格納する配列（[0]: 左輪　、[1]: 右輪）
 */
void calcStraightSpeed(float speed, float speeds[2]);

/**
 * 曲線走行の速度を計算する
 * @param angular_speed 角速度(°/s)
 * @param is_right 右曲がりか左曲がりか
 * @param radius 半径(mm)
 * @param speeds 速度(mm/s)を格納する配列（[0]: 左輪、[1]: 右輪）
 */
void calculateCurveSpeeds(float angular_speed, bool is_right, float radius, float speeds[2]);

#endif