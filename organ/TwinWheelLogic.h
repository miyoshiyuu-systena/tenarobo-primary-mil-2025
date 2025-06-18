#ifndef TWIN_WHEEL_LOGIC_H
#define TWIN_WHEEL_LOGIC_H

/**
 * 曲線走行の速度を計算する
 * @param angular_speed 角速度(°/s)
 * @param radius 半径(mm)
 * @param speeds 速度(mm/s)を格納する配列（[0]: 内輪　、[1]外輪）
 */
void calculateCurveSpeeds(float angular_speed, float radius, float speeds[2], bool is_right_side);

#endif