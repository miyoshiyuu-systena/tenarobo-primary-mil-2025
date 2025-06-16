#include "two_wheel_drive.h"
#include <cmath>
#include <algorithm>
#include "syssvc/syslog.h"

using   namespace   spikeapi;

TwoWheelDrive::TwoWheelDrive(Motor* left_motor, Motor* right_motor)
    : mLeftMotor(left_motor)
    , mRightMotor(right_motor)
    , mHasError(false)
{
    // モーターポインタの有効性チェック
    if (mLeftMotor == nullptr || mRightMotor == nullptr) {
        syslog(LOG_ERROR, "TwoWheelDrive: モーターポインタが無効です\n");
        mHasError = true;
        return;
    }
    
    // モーターの初期化が正常に完了したかチェック
    if (mLeftMotor->hasError() || mRightMotor->hasError()) {
        syslog(LOG_ERROR, "TwoWheelDrive: モーターの初期化が正常に完了していません\n");
        mHasError = true;
    }
}

bool TwoWheelDrive::hasError() const
{
    return mHasError;
}

void TwoWheelDrive::leftForward(int speed)
{
    if (mHasError || mLeftMotor == nullptr) return;
    mLeftMotor->setSpeed(speed);
}

void TwoWheelDrive::rightForward(int speed)
{
    if (mHasError || mRightMotor == nullptr) return;
    mRightMotor->setSpeed(speed);
}

void TwoWheelDrive::leftReverse(int speed)
{
    if (mHasError || mLeftMotor == nullptr) return;
    mLeftMotor->setSpeed(-speed);
}

void TwoWheelDrive::rightReverse(int speed)
{
    if (mHasError || mRightMotor == nullptr) return;
    mRightMotor->setSpeed(-speed);
}

void TwoWheelDrive::curveLeft(int speed, float radius)
{
    if (mHasError || mLeftMotor == nullptr || mRightMotor == nullptr || radius <= 0.0f) return;
    
    // 左曲線：右輪が外輪、左輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    int outer_speed = static_cast<int>(speed * outer_radius / radius);
    int inner_speed = static_cast<int>(speed * inner_radius / radius);
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0) {
        inner_speed = 0;
    }
    
    // 右輪（外輪）と左輪（内輪）の速度を設定
    mRightMotor->setSpeed(outer_speed);
    mLeftMotor->setSpeed(inner_speed);
}

void TwoWheelDrive::curveRight(int speed, float radius)
{
    if (mHasError || mLeftMotor == nullptr || mRightMotor == nullptr || radius <= 0.0f) return;
    
    // 右曲線：左輪が外輪、右輪が内輪
    float outer_radius = radius + WHEEL_TREAD_MM / 2.0f;
    float inner_radius = radius - WHEEL_TREAD_MM / 2.0f;
    
    int outer_speed = static_cast<int>(speed * outer_radius / radius);
    int inner_speed = static_cast<int>(speed * inner_radius / radius);
    
    // 内輪の速度が負になる場合は0にする
    if (inner_speed < 0) {
        inner_speed = 0;
    }
    
    // 左輪（外輪）と右輪（内輪）の速度を設定
    mLeftMotor->setSpeed(outer_speed);
    mRightMotor->setSpeed(inner_speed);
}

void TwoWheelDrive::setPower(int left_power, int right_power)
{
    if (mHasError || mLeftMotor == nullptr || mRightMotor == nullptr) return;
    
    // PWM値の範囲チェック（-100〜100）
    if (!isValidRange(left_power, -100, 100) || !isValidRange(right_power, -100, 100)) {
        return;
    }
    
    mLeftMotor->setPower(left_power);
    mRightMotor->setPower(right_power);
}

void TwoWheelDrive::stop()
{
    if (mHasError || mLeftMotor == nullptr || mRightMotor == nullptr) return;
    
    mLeftMotor->stop();
    mRightMotor->stop();
}

void TwoWheelDrive::brake()
{
    if (mHasError || mLeftMotor == nullptr || mRightMotor == nullptr) return;
    
    mLeftMotor->brake();
    mRightMotor->brake();
}

void TwoWheelDrive::resetLeftCount()
{
    if (mHasError || mLeftMotor == nullptr) return;
    
    mLeftMotor->resetCount();
}

void TwoWheelDrive::resetRightCount()
{
    if (mHasError || mRightMotor == nullptr) return;
    
    mRightMotor->resetCount();
}

int32_t TwoWheelDrive::getLeftCount() const
{
    if (mHasError || mLeftMotor == nullptr) return 0;
    
    return mLeftMotor->getCount();
}

int32_t TwoWheelDrive::getRightCount() const
{
    if (mHasError || mRightMotor == nullptr) return 0;
    
    return mRightMotor->getCount();
}

bool TwoWheelDrive::isValidRange(int value, int min, int max) const
{
    return (value >= min && value <= max);
} 