#include "TwinWheelDrive.h"
#include "TwinWheelConst.h"
#include "Motor.h"
#include "Port.h"

TwinWheelDrive::TwinWheelDrive(EPort left_port, EPort right_port)
    /**             ポート番号        モーターの回転方向(時計/反時計)               インスタンス化するときにカウントをリセットするか */
    : mLeftMotor(   left_port,      Motor::EDirection::COUNTERCLOCKWISE,    true    )
    , mRightMotor(  right_port,     Motor::EDirection::CLOCKWISE,           true    )
{}

bool TwinWheelDrive::hasError()
{
    return mLeftMotor.hasError() || mRightMotor.hasError();
}

void TwinWheelDrive::leftPivotTurn(int angular_speed)
{
    // 左超信地回転：左輪逆回転、右輪正回転（その場回転）
    // ロボットの回転半径 = 車輪間距離の半分
    // 車輪の線速度 = angular_speed * (WHEEL_TREAD_MM / 2) * (π / 180) [mm/s]
    // 車輪の角速度 = 線速度 * 360 / (π * WHEEL_DIAMETER_MM) [°/s]
    // 簡略化: motor_speed = angular_speed * WHEEL_TREAD_MM / WHEEL_DIAMETER_MM
    
    int motor_speed = static_cast<int>(angular_speed * WHEEL_TREAD_MM / WHEEL_DIAMETER_MM);
    
    mLeftMotor.setSpeed(motor_speed);
    mRightMotor.setSpeed(-motor_speed);
}

void TwinWheelDrive::rightPivotTurn(int angular_speed)
{
    // 右超信地回転：左輪正回転、右輪逆回転（その場回転）
    // 計算は左回転と同じ
    
    int motor_speed = static_cast<int>(angular_speed * WHEEL_TREAD_MM / WHEEL_DIAMETER_MM);
    
    mLeftMotor.setSpeed(-motor_speed);
    mRightMotor.setSpeed(motor_speed);
}

void TwinWheelDrive::leftSpinTurn(int angular_speed)
{
    // 左信地回転：右輪中心で左回転（右輪停止、左輪回転）
    // ロボットの回転半径 = 車輪間距離
    // 車輪の線速度 = angular_speed * WHEEL_TREAD_MM * (π / 180) [mm/s]
    // 車輪の角速度 = 線速度 * 360 / (π * WHEEL_DIAMETER_MM) [°/s]
    // 簡略化: motor_speed = angular_speed * WHEEL_TREAD_MM * 2 / WHEEL_DIAMETER_MM

    int motor_speed = static_cast<int>(angular_speed * WHEEL_TREAD_MM * 2.0f / WHEEL_DIAMETER_MM);
    
    mLeftMotor.setSpeed(motor_speed);
    mRightMotor.setSpeed(0);
}

void TwinWheelDrive::rightSpinTurn(int angular_speed)
{
    // 右信地回転：左輪中心で右回転（左輪停止、右輪回転）
    // 計算は左信地回転と同じ
    
    int motor_speed = static_cast<int>(angular_speed * WHEEL_TREAD_MM * 2.0f / WHEEL_DIAMETER_MM);
    
    mLeftMotor.setSpeed(0);
    mRightMotor.setSpeed(motor_speed);
}

void TwinWheelDrive::setSpeed(float left_speed, float right_speed)
{
    mLeftMotor.setSpeed(left_speed);
    mRightMotor.setSpeed(right_speed);
}

void TwinWheelDrive::stop()
{
    mLeftMotor.setSpeed(0);
}

void TwinWheelDrive::brake()
{
    mLeftMotor.brake();
    mRightMotor.brake();
}

void TwinWheelDrive::resetCount()
{
    mLeftMotor.resetCount();
}

void TwinWheelDrive::resetCount()
{
    mLeftMotor.resetCount();
    mRightMotor.resetCount();
}

void TwinWheelDrive::getLeftSpeed() const
{
    return mLeftMotor.getSpeed();
}

void TwinWheelDrive::getRightSpeed() const
{
    return mRightMotor.getSpeed();
}