#include "TwinWheelDrive.h"
#include <cmath>
#include <algorithm>

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
    // 左超信地回転：左輪逆回転、右輪正回転
    // 車輪間距離の半分の円周上を回転
    float wheel_speed = angular_speed * (WHEEL_TREAD_MM / 2.0f) * (M_PI / 180.0f) / (WHEEL_DIAMETER_MM * M_PI / 2.0f);
    int motor_speed = static_cast<int>(wheel_speed * 360.0f / (M_PI * WHEEL_DIAMETER_MM)); 
    
    mLeftMotor.setSpeed(-motor_speed);
    mRightMotor.setSpeed(motor_speed);
}

void TwinWheelDrive::rightPivotTurn(int angular_speed)
{
    // 右超信地回転：左輪正回転、右輪逆回転
    // 車輪間距離の半分の円周上を回転
    float wheel_speed = angular_speed * (WHEEL_TREAD_MM / 2.0f) * (M_PI / 180.0f) / (WHEEL_DIAMETER_MM * M_PI / 2.0f);
    int motor_speed = static_cast<int>(wheel_speed * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    
    mLeftMotor.setSpeed(motor_speed);
    mRightMotor.setSpeed(-motor_speed);
}

void TwinWheelDrive::leftSpinTurn(int angular_speed)
{
    // 左信地回転：右輪中心で左回転（右輪停止、左輪回転）
    // 車輪間距離の円周上を回転
    float wheel_speed = angular_speed * WHEEL_TREAD_MM * (M_PI / 180.0f) / (WHEEL_DIAMETER_MM * M_PI / 2.0f);
    int motor_speed = static_cast<int>(wheel_speed * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    
    mLeftMotor.setSpeed(motor_speed);
    mRightMotor.setSpeed(0);
}

void TwinWheelDrive::rightSpinTurn(int angular_speed)
{
    // 右信地回転：左輪中心で右回転（左輪停止、右輪回転）
    // 車輪間距離の円周上を回転
    float wheel_speed = angular_speed * WHEEL_TREAD_MM * (M_PI / 180.0f) / (WHEEL_DIAMETER_MM * M_PI / 2.0f);
    int motor_speed = static_cast<int>(wheel_speed * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    
    mLeftMotor.setSpeed(0);
    mRightMotor.setSpeed(motor_speed);
}

void TwinWheelDrive::curveLeftSpeed(float travel_speed, float radius)
{
    // 左曲線：右輪が外輪、左輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    // 走行速度(mm/s)から角速度(°/s)に変換
    float outer_speed_mms = travel_speed * outer_radius / radius;
    float inner_speed_mms = travel_speed * inner_radius / radius;
    
    // mm/sから°/sに変換
    int outer_speed = static_cast<int>(outer_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    int inner_speed = static_cast<int>(inner_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0) {
        inner_speed = 0;
    }
    
    // 右輪（外輪）と左輪（内輪）の速度を設定
    mRightMotor.setSpeed(outer_speed);
    mLeftMotor.setSpeed(inner_speed);
}

void TwinWheelDrive::curveRightSpeed(float travel_speed, float radius)
{
    // 右曲線：左輪が外輪、右輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    // 走行速度(mm/s)から角速度(°/s)に変換
    float outer_speed_mms = travel_speed * outer_radius / radius;
    float inner_speed_mms = travel_speed * inner_radius / radius;
    
    // mm/sから°/sに変換
    int outer_speed = static_cast<int>(outer_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    int inner_speed = static_cast<int>(inner_speed_mms * 360.0f / (M_PI * WHEEL_DIAMETER_MM));
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0) {
        inner_speed = 0;
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

bool TwinWheelDrive::isValidRange(int value, int min, int max) const
{
    return (value >= min && value <= max);
} 