#ifndef _PERCEPTION_DATA_ACCESS_H_
#define _PERCEPTION_DATA_ACCESS_H_

/**
 * 知覚データへのアクセス
 * 構造体として定義する
 */
struct PerceptionDataAccess
{
    /**
     * 力センサーの値
     */
    float force;

    /**
     * 超音波センサーの値
     */
    float distance;

    /**
     * カラーセンサーの値
     */
    int color[3];

    /**
     * 左モーターの値
     */
    int leftMotor;

    /**
     * 右モーターの値
     */
    int rightMotor;

    // ここに知覚データのアクセスを定義する
};

/**
 * 知覚データへのアクセス
 * グローバルデータとしてインスタンス化する
 */
PerceptionDataAccess perceptionDataAccess = {
    .force = 0.0f,
    .distance = 0.0f,
    .color = {0, 0, 0},
    .leftMotor = 0,
    .rightMotor = 0
};

#endif