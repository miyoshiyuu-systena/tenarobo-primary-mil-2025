#ifndef _PERCEPTION_DATA_ACCESS_H_
#define _PERCEPTION_DATA_ACCESS_H_

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

#endif