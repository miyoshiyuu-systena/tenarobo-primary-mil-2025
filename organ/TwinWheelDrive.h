#ifndef _TWO_WHEEL_DRIVE_H_
#define _TWO_WHEEL_DRIVE_H_

#include <cstdint>
#include "Motor.h"
#include "Port.h"

using   namespace   spikeapi;

/**
 * 2輪駆動システムクラス
 */
class TwoWheelDrive
{
public:
    /**
     * コンストラクタ
     * @param left_motor 左モーターのポインタ
     * @param right_motor 右モーターのポインタ
     */
    TwoWheelDrive(Motor* left_motor, Motor* right_motor);

    /**
     * 初期化が正常に完了したかどうかを確認
     * @return true 正常に初期化された
     * @return false 初期化に失敗した
     */
     // @note   このメソッドは正しく動作しない可能性が高い
     //         モーターは初期化が完了しても、hasError = trueを返す
    bool hasError() const;

    /**
     * 左正転回転
     * @param speed 速度（°/s）
     */
    void leftForward(int speed);

    /**
     * 右正転回転
     * @param speed 速度（°/s）
     */
    void rightForward(int speed);

    /**
     * 左逆転回転
     * @param speed 速度（°/s）
     */
    void leftReverse(int speed);

    /**
     * 右逆転回転
     * @param speed 速度（°/s）
     */
    void rightReverse(int speed);

    /**
     * 左曲線走行
     * @param speed 速度（°/s）
     * @param radius 半径（mm）
     */
    void curveLeft(int speed, float radius);

    /**
     * 右曲線走行
     * @param speed 速度（°/s）
     * @param radius 半径（mm）
     */
    void curveRight(int speed, float radius);

    /**
     * 両輪へPWM値を指示
     * @param left_power 左モーターへのPWM値（-100〜100）
     * @param right_power 右モーターへのPWM値（-100〜100）
     */
    void setPower(int left_power, int right_power);

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

private:
    static constexpr float WHEEL_TREAD_MM = 115.0f; /**< 車輪間距離（トレッド）[mm] */

    Motor* mLeftMotor;   /**< 左モーター */
    Motor* mRightMotor;  /**< 右モーター */
    bool mHasError;      /**< エラーフラグ */

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