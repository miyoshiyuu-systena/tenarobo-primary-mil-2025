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

    /**
     * 前腕のエンコーダの値を取得
     * @return 前腕のエンコーダの値[カウント]
     */
    int getCount() const;

    /**
     * 前腕のエンコーダの値をリセット
     */
    void resetCount();

    /**
     * 前腕のエンコーダの値を保って停止
     */
    void hold();

    /**
     * ストールを確認する
     * 
     * @note
     *  ストールとは、モーターがなんらかの原因で動いていないことを指す
     *  ストールが発生する主な原因：
     *  物理的な障害物 - モーターに接続された機構が壁や物体にぶつかって動けない
     *  過負荷 - モーターにかかる負荷が大きすぎて回転できない
     *  機械的な問題 - ギアの噛み合いや軸の問題で動作が阻害される
     *  電力不足 - 十分な電力が供給されていない
     * 
     * @return 前腕がストールしているかどうか
     */
    bool isStalled() const;

private:
    Motor mMotor;
};

#endif // FRONT_ARM_DRIVE_H