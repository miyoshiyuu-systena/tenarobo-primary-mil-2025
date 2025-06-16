#ifndef _MODULE_ACCESS_H_
#define _MODULE_ACCESS_H_

#include "TwinWheelDrive.h"
#include "Motor.h"
#include "ForceSensor.h"
#include "ColorSensor.h"
#include "UltrasonicSensor.h"
#include "IMU.h"

using namespace spikeapi;

// 他の周辺モジュールのインスタンスを宣言
TwinWheelDrive twinWheelDrive(
    EPort::PORT_B,      // 左モーターのポート
    EPort::PORT_A       // 右モーターのポート
);

/**
 * 前腕モーター
 */
Motor frontArm(
    EPort::PORT_C,                  // 前腕モーターのポート
    Motor::EDirection::CLOCKWISE,   // 前腕モーターの方向
    true                            // 前腕モーターのカウンタリセット
);

/**
 * 力センサー
 */
ForceSensor forceSensor(EPort::PORT_D);

/**
 * カラーセンサー
 */
ColorSensor colorSensor(EPort::PORT_E);

/**
 * 超音波センサー
 */
UltrasonicSensor ultrasonicSensor(EPort::PORT_F);

/**
 * ハブ内蔵IMU
 */
IMU imu;

#endif