#ifndef _MODULE_ACCESS_H_
#define _MODULE_ACCESS_H_

#include "TwinWheelDrive.h"
#include "Motor.h"
#include "ForceSensor.h"
#include "ColorSensor.h"
#include "UltrasonicSensor.h"
#include "IMU.h"

using namespace spikeapi;

// 他の周辺モジュールのインスタンスを宣言（実体は.cppファイルで定義）
extern TwinWheelDrive twinWheelDrive;

/**
 * 前腕モーター
 */
extern Motor frontArm;

/**
 * 力センサー
 */
extern ForceSensor forceSensor;

/**
 * カラーセンサー
 */
extern ColorSensor colorSensor;

/**
 * 超音波センサー
 */
extern UltrasonicSensor ultrasonicSensor;

/**
 * ハブ内蔵IMU
 */
extern IMU imu;

#endif