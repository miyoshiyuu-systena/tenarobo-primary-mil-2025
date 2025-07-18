#ifndef _TWO_WHEEL_DRIVE_H_
#define _TWO_WHEEL_DRIVE_H_

#include <cstdint>
#include "Motor.h"
#include "Port.h"

using   namespace   spikeapi;

/**
 * 2輪駆動システムクラス
 */
class TwinWheelDrive
{
public:
    /**
     * コンストラクタ
     * @param left_port 左モーターのポート
     * @param right_port 右モーターのポート
     */
    TwinWheelDrive(EPort left_port, EPort right_port);

    /**
     * 初期化が正常に完了したかどうかを確認
     * @return true 正常に初期化された
     * @return false 初期化に失敗した
     */
     // @note   このメソッドは正しく動作しない可能性が高い
     //         モーターは初期化が完了しても、hasError = trueを返す
    bool hasError();
    
    /**
     * 左超信地回転（その場で左回転）
     * @param angular_speed 回転速度（°/s）
     */
    void leftPivotTurn(int angular_speed);

    /**
     * 右超信地回転（その場で右回転）
     * @param angular_speed 回転速度（°/s）
     */
    void rightPivotTurn(int angular_speed);

    /**
     * 左信地回転（右輪中心の左回転）
     * @param angular_speed 回転速度（°/s）
     */
    void leftSpinTurn(int angular_speed);

    /**
     * 右信地回転（左輪中心の右回転）
     * @param angular_speed 回転速度（°/s）
     */
    void rightSpinTurn(int angular_speed);

    /**
     * 左曲線走行（走行速度指定版）
     * @param angular_speed 回転速度（°/s）
     * @param radius 半径（mm）
     * @note 進行方向から真左radiusの位置に中心があると仮想し、回転速度angular_speedで円弧を描くように移動する
     */
    void curveLeftSpeed(float angular_speed, float radius);

    /**
     * 右曲線走行（走行速度指定版）
     * @param angular_speed 回転速度（°/s）
     * @param radius 半径（mm）
     * @note 進行方向から真右radiusの位置に中心があると仮想し、回転速度angular_speedで円弧を描くように移動する
     */
    void curveRightSpeed(float angular_speed, float radius);

    /**
     * 両輪へPWM値を指示
     * @param left_power 左モーターへのPWM値（-100〜100）
     * @param right_power 右モーターへのPWM値（-100〜100）
     */
    void setPower(int left_power, int right_power);
    
    /**
     * 両輪へスピードを指示
     * @param left_speed 左モーターへのスピード（°/秒）
     * @param right_speed 右モーターへのスピード（°/秒）
     */
    void setSpeed(float left_speed, float right_speed);

    /**
     * 2輪駆動システムの停止
     */
    void stop();

    /**
     * 2輪駆動システムのブレーキ
     */
    void brake();

    /**
     * 左モーターのエンコーダリセット
     */
    void resetLeftCount();

    /**
     * 右モーターのエンコーダリセット
     */
    void resetRightCount();

    /**
     * 左モーターのエンコーダ値取得
     * @return エンコーダ値 [°]
     */
    int32_t getLeftCount() const;

    /**
     * 右モーターのエンコーダ値取得
     * @return エンコーダ値 [°]
     */
    int32_t getRightCount() const;

    /**
     * 左モーターの回転速度取得
     * @return 回転速度（°/s）
     */
    float getLeftSpeed() const;

    /**
     * 右モーターの回転速度取得
     * @return 回転速度（°/s）
     */
    float getRightSpeed() const;

    /**
     * 左モーターのPWM値
     * @return PWM値（-100~100）
     */
    int getLeftPower() const;

    /**
     * 右モーターのPWM値
     * @return PWM値（-100~100）
     */
    int getRightPower() const;

private:
    Motor mLeftMotor;   /**< 左モーター */
    Motor mRightMotor;  /**< 右モーター */

    /**
     * 引数の有効性を確認
     * @param value 確認する値
     * @param min 最小値
     * @param max 最大値
     * @return true 有効な値
     * @return false 無効な値
     */
    bool isValidRange(int value, int min, int max) const;
};

#endif // _TWO_WHEEL_DRIVE_H_ 