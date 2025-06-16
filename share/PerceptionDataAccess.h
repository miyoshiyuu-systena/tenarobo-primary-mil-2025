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
     * 単位: N
     */
    float force;

    /**
     * 超音波センサーの値
     * 単位: mm
     */
    int distance;

    /**
     * カラーセンサーの値
     * H: 色相
     * S: 彩度
     * V: 明度
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
 * グローバルデータとして宣言する（実体は.cppファイルで定義）
 */
extern PerceptionDataAccess perceptionDataAccess;

#endif