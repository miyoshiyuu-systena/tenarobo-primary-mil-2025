#include "perception/Perception.h"
#include "device/Device.h"
#include "debug.h"
#include "spikeapi.h"
#include "logger/Logger.h"
#include "web-camera/CameraManager.h"

using namespace spikeapi;

Perception::Perception()
    : count_cycle(0),
      mFrontImage(cv::Mat()),
      mDistance(-1),
      mColorH(-1),
      mColorS(-1),
      mColorV(-1),
      mForce(-1.0f),
      mLeftMotorSpeed(-1.0f),
      mRightMotorSpeed(-1.0f),
      mFrontArmSpeed(-1.0f),
      mMask(0b11111111)
{
}

int Perception::getDistance() const {
    return mDistance;
}

int Perception::getColorH() const {
    return mColorH;
}

int Perception::getColorS() const {
    return mColorS;
}

int Perception::getColorV() const {
    return mColorV;
}

float Perception::getForce() const {
    return mForce;
}

float Perception::getLeftMotorSpeed() const {
    return mLeftMotorSpeed;
}

float Perception::getRightMotorSpeed() const {
    return mRightMotorSpeed;
}

float Perception::getFrontArmSpeed() const {
    return mFrontArmSpeed;
}

cv::Mat Perception::getFrontImage() const {
    return mFrontImage;
}

void Perception::setMask(uint8_t mask) {
    mMask = mask;

    // マスクされたセンサの値は初期化する
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
        mFrontImage = cv::Mat();
    }
}

void Perception::update() {
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
        mColorH = hsv.h;
        mColorS = hsv.s;
        mColorV = hsv.v;
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
        mLeftMotorSpeed = twinWheelDrive.getLeftSpeed();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_RIGHT_MOTOR) != 0b00000000
    ) {
        mRightMotorSpeed = twinWheelDrive.getRightSpeed();
    }

    if (
        isPerceptionLoggingIgnoreMask ||
        (mMask & MASK_FRONT_ARM) != 0b00000000
    ) {
        mFrontArmSpeed = frontArmDrive.getSpeed();
    }

    // カメラ処理の頻度を下げる
    if (count_cycle % cameraSaveInterval == 0) {
        loc_cpu();
        /**
         * カメラデータの取得
         */
        static CameraManager& cameraManager = CameraManager::getInstance();
        static bool initAttempted = false;

        /**
         * XXX: メインタスクで実装できないか
         */
        // カメラが初期化されていない場合は初期化を試行（一度だけ）
        if (!cameraManager.isInitialized() && !initAttempted) {
            cameraManager.initializeCamera();
            initAttempted = true;
        }
        
        // カメラが初期化されている場合のみ処理
        if (cameraManager.isInitialized()) {
            // 1フレーム取得して保存
            cv::Mat frame;
            if (cameraManager.captureImageNow(frame)) {
                // 画像保存の頻度も下げる（100回→500回に1回）
                cameraManager.saveImage(frame, "perc_task");
            }
        }
        unl_cpu();
    }

    if (isPerceptionLoggingEnable) {
        Logger& logger = Logger::getInstance();

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

Perception perception;