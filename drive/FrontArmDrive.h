#ifndef FRONT_ARM_DRIVE_H
#define FRONT_ARM_DRIVE_H

#include "Port.h"
#include "Motor.h"

using namespace spikeapi;

/**
 * 前腕駆動クラス
 * このクラスの仕事はロボットの前腕を動かすことである
 * 
 * @note
 *  このロボットは前腕を持つ
 *  前腕は2輪と同じくMotorクラスを使用して制御できる
 *  前腕を使用して、正面の対象物を移動させるタスクを想定している
 */
class FrontArmDrive
{
public:
    /**
     * コンストラクタ
     * @param port モーターのポート
     */
    FrontArmDrive(EPort port);

    /**
     * デストラクタ
     */
    ~FrontArmDrive();

    /**
     * TODO: 前腕の指示インターフェースの検討
     * TODO: 前腕の指示インターフェースの実装
     */
    void setSpeed(float speed);

    /**
     * 前腕の回転速度を取得
     * @return 前腕の回転速度[°/s]
     */
    float getSpeed() const;

private:
    Motor mMotor;
};

#endif // FRONT_ARM_DRIVE_H