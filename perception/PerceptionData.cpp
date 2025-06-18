#include "PerceptionData.h"
#include "Device.h"
#include "debug.h"

using namespace spikeapi;

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
#define MASK_CAMERA         0b10000000
#define MASK_ULTRASONIC     0b01000000
#define MASK_COLOR          0b00100000
#define MASK_FORCE          0b00010000
#define MASK_LEFT_MOTOR     0b00001000
#define MASK_RIGHT_MOTOR    0b00000100
#define MASK_FRONT_ARM      0b00000010
#define MASK_IMU            0b00000001

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

    /**
     * マスクされたセンサの値は初期化する
     */
    
    if ((mMask & MASK_ULTRASONIC) == 0b00000000) {
        mDistance = -1;
    }

    if ((mMask & MASK_COLOR) == 0b00000000) {
        mColorH = -1;
        mColorS = -1;
        mColorV = -1;
    }

    if ((mMask & MASK_FORCE) == 0b00000000) {
        mForce = -1.0f;
    }

    if ((mMask & MASK_LEFT_MOTOR) == 0b00000000) {
        mLeftMotorSpeed = -1.0f;
    }

    if ((mMask & MASK_RIGHT_MOTOR) == 0b00000000) {
        mRightMotorSpeed = -1.0f;
    }

    if ((mMask & MASK_FRONT_ARM) == 0b00000000) {
        mFrontArmSpeed = -1.0f;
    }

    if ((mMask & MASK_CAMERA) == 0b00000000) {
        // mFrontImage = -1;
    }
}

void perception_task()
{
    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_ULTRASONIC) != 0b00000000
    ) {
        mDistance = ultrasonicSensor.getDistance();
    }
    
    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_COLOR) != 0b00000000
    ) {
        ColorSensor::HSV hsv;
        colorSensor.getHSV(hsv, true);
        loc_cpu();
        mColorH = hsv.h;
        mColorS = hsv.s;
        mColorV = hsv.v;
        unl_cpu();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_FORCE) != 0b00000000
    ) {
        mForce = forceSensor.getForce();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_LEFT_MOTOR) != 0b00000000
    ) {
        mLeftMotorSpeed = twinWheelDrive.getLeftMotorSpeed();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_RIGHT_MOTOR) != 0b00000000
    ) {
        mRightMotorSpeed = twinWheelDrive.getRightMotorSpeed();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_FRONT_ARM) != 0b00000000
    ) {
        mFrontArmSpeed = frontArm.getSpeed();
    }
    
    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_CAMERA) != 0b00000000 &&
        count_cycle % cameraSaveInterval == 0   // カメラの保存間隔
    ) {
        // loc_cpu();
        // // TODO: カメラの画像を保存
        // /**
        //  * カメラデータの取得
        //  */
        //  static CameraManager& cameraManager = CameraManager::getInstance();
        //  static bool initAttempted = false;
 
        //  /**
        //   * XXX: メインタスクで実装できないか
        //   */
        //  // カメラが初期化されていない場合は初期化を試行（一度だけ）
        //  if (!cameraManager.isInitialized() && !initAttempted) {
        //      cameraManager.initializeCamera();
        //      initAttempted = true;
        //  }
         
        //  // カメラが初期化されている場合のみ処理
        //  if (cameraManager.isInitialized()) {
        //      // 1フレーム取得して保存
        //      cv::Mat frame;
        //      if (cameraManager.captureImageNow(frame)) {
        //          // 画像保存の頻度も下げる（100回→500回に1回）
        //          cameraManager.saveImage(frame, "perc_task");
        //      }
        //  }
        // unl_cpu();
    }

    if (isPerceptionLoggingEnable) {
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