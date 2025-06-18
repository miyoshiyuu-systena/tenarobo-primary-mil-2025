#include "PerceptionData.h"
#include "Device.h"

using namespace spikeapi;

/**
 * ログ出力の有効/無効
 * デバッグ用
 */
static const bool isLoggingEnable = false;

/**
 * カメラの保存間隔
 */
static const int cameraSaveInterval = 1000;

/**
 * 知覚タスクのサイクル回数
 * 測定を間引きするために使用
 */
static int count_cycle = 0;

// static int mFrontImage = -1;
static int mDistance = -1;
static int mColorH = -1;
static int mColorS = -1;
static int mColorV = -1;
static float mForce = -1.0f;
static float mLeftMotorSpeed = -1.0f;
static float mRightMotorSpeed = -1.0f;
static float mFrontArmSpeed = -1.0f;

/**
 * データ更新を宣言するマスク（逆に言うと更新する必要のないデータは更新しない)
 * 0: 更新しない
 * 1: 更新する
 * 
 * [7] Webカメラ
 * [6] 超音波センサー
 * [5] カラーセンサー
 * [4] 圧力センサー
 * [3] 左モーター
 * [2] 右モーター
 * [1] 前腕モーター
 * [0] ハブ内蔵IMU
 */
static uint8_t mMask = 0b00000000;
#define MASK_CAMERA         (1 << 7)
#define MASK_ULTRASONIC     (1 << 6)
#define MASK_COLOR          (1 << 5)
#define MASK_FORCE          (1 << 4)
#define MASK_LEFT_MOTOR     (1 << 3)
#define MASK_RIGHT_MOTOR    (1 << 2)
#define MASK_FRONT_ARM      (1 << 1)
#define MASK_IMU            (1 << 0)

// int getFrontImage()
// {
//     return mFrontImage;
// }

int getDistance()
{
    return mDistance;
}

int getColorH()
{
    return mColorH;
}

int getColorS()
{
    return mColorS;
}

int getColorV()
{
    return mColorV;
}

float getForce()
{
    return mForce;
}

float getLeftMotorSpeed()
{
    return mLeftMotorSpeed;
}

float getRightMotorSpeed()
{
    return mRightMotorSpeed;
}

float getFrontArmSpeed()
{
    return mFrontArmSpeed;
}

void setMask(uint8_t mask)
{
    mMask = mask;
}

void perception_task()
{
    if (mMask & MASK_ULTRASONIC) {
        mDistance = ultrasonicSensor.getDistance();
    }
    
    if (mMask & MASK_COLOR) {
        ColorSensor::HSV hsv;
        colorSensor.getHSV(hsv, true);
        loc_cpu();
        mColorH = hsv.h;
        mColorS = hsv.s;
        mColorV = hsv.v;
        unl_cpu();
    }

    if (mMask & MASK_FORCE) {
        mForce = forceSensor.getForce();
    }

    if (mMask & MASK_LEFT_MOTOR) {
        mLeftMotorSpeed = twinWheelDrive.getLeftMotorSpeed();
    }

    if (mMask & MASK_RIGHT_MOTOR) {
        mRightMotorSpeed = twinWheelDrive.getRightMotorSpeed();
    }

    if (mMask & MASK_FRONT_ARM) {
        mFrontArmSpeed = frontArm.getSpeed();
    }
    
    if (
        mMask & MASK_CAMERA &&
        count_cycle % cameraSaveInterval == 0   // カメラの保存間隔
    ) {
        // TODO: カメラの画像を保存
        // loc_cpu();
        // カメラの画像を保存
        // mFrontImage = webCamera.getImage();
        // unl_cpu();
    }

    if (isLoggingEnable) {
        Logger logger = Logger::getInstance();

        logger.logDebug("--------------------------------");
        logger.logDebug(
            "Distance: " + std::to_string(mDistance) +
            ", ColorH: " + std::to_string(mColorH) +
            ", ColorS: " + std::to_string(mColorS) +
            ", ColorV: " + std::to_string(mColorV) +
            ", Force: " + std::to_string(mForce)
        );
        logger.logDebug(
            "LeftMotorSpeed: " + std::to_string(mLeftMotorSpeed) +
            ", RightMotorSpeed: " + std::to_string(mRightMotorSpeed) +
            ", FrontArmSpeed: " + std::to_string(mFrontArmSpeed)
        );
        logger.logDebug("--------------------------------");
    }

    count_cycle++;
}