#include "TwinWheelDrive.h"
#include <cmath>
#include <algorithm>
#include "TwinWheelConst.h"

using   namespace   spikeapi;

TwinWheelDrive::TwinWheelDrive(EPort left_port, EPort right_port)
    : mLeftMotor(left_port, Motor::EDirection::COUNTERCLOCKWISE, true)
    , mRightMotor(right_port, Motor::EDirection::CLOCKWISE, true)
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

void TwinWheelDrive::curveLeftSpeed(float angular_speed, float radius)
{
    // 半径が車輪間隔の半分より小さい場合は停止
    if (radius < WHEEL_TREAD_MM / 2.0f) {
        stop();
        return;
    }
    
    // 左曲線：右輪が外輪、左輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    // 角速度(°/s)から線速度(mm/s)に変換
    float outer_speed_mms = angular_speed * outer_radius * (M_PI / 180.0f);
    float inner_speed_mms = angular_speed * inner_radius * (M_PI / 180.0f);
    
    // mm/sから°/sに変換
    float outer_speed = outer_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    float inner_speed = inner_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0.0f) {
        inner_speed = 0.0f;
    }

    // 右輪（外輪）と左輪（内輪）の速度を設定
    mRightMotor.setSpeed(outer_speed);
    mLeftMotor.setSpeed(inner_speed);
}

void TwinWheelDrive::curveRightSpeed(float angular_speed, float radius)
{
    // 半径が車輪間隔の半分より小さい場合は停止
    if (radius < WHEEL_TREAD_MM / 2.0f) {
        stop();
        return;
    }
    
    // 右曲線：左輪が外輪、右輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    // 角速度(°/s)から線速度(mm/s)に変換
    float outer_speed_mms = angular_speed * outer_radius * (M_PI / 180.0f);
    float inner_speed_mms = angular_speed * inner_radius * (M_PI / 180.0f);
    
    // mm/sから°/sに変換
    float outer_speed = outer_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    float inner_speed = inner_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM);
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0.0f) {
        inner_speed = 0.0f;
    }
    
    // 左輪（外輪）と右輪（内輪）の速度を設定
    mLeftMotor.setSpeed(outer_speed);
    mRightMotor.setSpeed(inner_speed);
}

void TwinWheelDrive::setPower(int left_power, int right_power)
{
    // PWM値の範囲チェック（-100〜100）
    if (!isValidRange(left_power, -100, 100) || !isValidRange(right_power, -100, 100)) {
        return;
    }
    
    mLeftMotor.setPower(left_power);
    mRightMotor.setPower(right_power);
}

void TwinWheelDrive::setSpeed(float left_speed, float right_speed)
{
    mLeftMotor.setSpeed(left_speed);
    mRightMotor.setSpeed(right_speed);
}

void TwinWheelDrive::stop()
{
    mLeftMotor.stop();
    mRightMotor.stop();
}

void TwinWheelDrive::brake()
{
    mLeftMotor.brake();
    mRightMotor.brake();
}

void TwinWheelDrive::resetLeftCount()
{
    mLeftMotor.resetCount();
}

void TwinWheelDrive::resetRightCount()
{
    mRightMotor.resetCount();
}

int32_t TwinWheelDrive::getLeftCount() const
{
    return mLeftMotor.getCount();
}

int32_t TwinWheelDrive::getRightCount() const
{
    return mRightMotor.getCount();
}

float TwinWheelDrive::getLeftSpeed() const
{
    return mLeftMotor.getSpeed();
}

float TwinWheelDrive::getRightSpeed() const
{
    return mRightMotor.getSpeed();
}

int TwinWheelDrive::getLeftPower() const
{
    return mLeftMotor.getPower();
}

int TwinWheelDrive::getRightPower() const
{
    return mRightMotor.getPower();
}

bool TwinWheelDrive::isValidRange(int value, int min, int max) const
{
    return (value >= min && value <= max);
} 