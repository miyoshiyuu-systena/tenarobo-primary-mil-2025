#ifndef CALC_CURVE_DRIVE_SPEED_H
#define CALC_CURVE_DRIVE_SPEED_H

/**
 * 曲線走行の速度を計算する
 * @param angular_speed 角速度(°/s)
 * @param radius 半径(mm)
 * @param speeds 速度(mm/s)を格納する配列（[0]: 外輪、[1]: 内輪）
 * @note
 *  2輪の回転速度に差をつけて走行することで、曲線走行を実現する
 */
 void calcCurveSpeeds(float angular_speed, float radius, float speeds[2]);

#endif // CALC_CURVE_DRIVE_SPEED_H