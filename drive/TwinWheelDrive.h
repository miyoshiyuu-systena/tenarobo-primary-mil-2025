#ifndef TWIN_WHEEL_DRIVE_H
#define TWIN_WHEEL_DRIVE_H

#include "Port.h"
#include "Motor.h"

using namespace spikeapi;

/**
 * 2輪駆動クラス
 * このクラスの仕事はロボットの移動に関わるモーターを連動して、ロボットの走行を実現することである
 *
 * @note
 *  このロボットは前輪2輪駆動である
 *  RasPike-ARTから車輪を動かすためのMotorクラスは定義されているが、
 *  実際の走行を実現するためには、2つのMotorインスタンスを有機的に連動させる必要がある
 * 
 *  例えば、
 *  真っ直ぐに走行するためには、左右の輪の速度を同じにして、それぞれ逆方向に回転させる必要がある
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
     * @param angular_speed ロボット全体の回転速度[°/s]
     */
    void leftPivotTurn(int angular_speed);

    /**
     * 右超信地回転（その場で右回転）
     * @param angular_speed ロボット全体の回転速度[°/s]
     */
    void rightPivotTurn(int angular_speed);

    /**
     * 左信地回転（右輪中心の左回転）
     * @param angular_speed ロボット全体の回転速度[°/s]
     */
    void leftSpinTurn(int angular_speed);
    
    /**
     * 右信地回転（左輪中心の右回転）
     * @param angular_speed ロボット全体の回転速度[°/s]
     */
    void rightSpinTurn(int angular_speed);

    /**
     * 両輪へスピードを指示する
     * @param left_speed 左モーターへのスピード[°/s]（この速度は車輪が単位秒あたりに回転する角度を示している）
     * @param right_speed 右モーターへのスピード[°/s]（この速度は車輪が単位秒あたりに回転する角度を示している）
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
     * 両輪のエンコーダリセット
     * @note
     *  エンコーダの値は、モーターの回転数を示している
     *  このメソッドを実行すると、エンコーダの値が0にリセットされる
     *  元々の0の回転位置までモーターが回転するわけではなく、現在の回転角度をソフト的に0にする
     */
    void resetCount();

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

private:
    Motor mLeftMotor;
    Motor mRightMotor;
};

#endif // TWIN_WHEEL_DRIVE_H