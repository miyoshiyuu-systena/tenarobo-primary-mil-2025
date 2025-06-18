#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "drive/TwinWheelDrive.h"
#include "drive/FrontArmDrive.h"
#include "Motor.h"
#include "ForceSensor.h"
#include "ColorSensor.h"
#include "UltrasonicSensor.h"
#include "IMU.h"

using namespace spikeapi;

/**
 * デバイスのインスタンスを宣言
 * 
 * @note
 *  このファイルをインクルードしたファイルでは、デバイスにアクセスできるようになる
 */

/**
 * 二輪モーター
 */
extern TwinWheelDrive twinWheelDrive;

/**
 * 前腕モーター
 */
extern FrontArmDrive frontArmDrive;

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
 * 
 * @note 
 *  カラーセンサの床面に対する角度は前腕の角度と連動しているため
 *  必ずしも床面から鉛直方向の反射光を取得できているわけではない
 */
extern UltrasonicSensor ultrasonicSensor;

/**
 * Webカメラ
 * @note
 *  このデバイスだけ、Spike経由の制御ではない
 *  自動初期化されないため、自身で実装する必要がある
 */
// extern WebCamera webCamera;

/**
 * ハブ内蔵IMU
 * @note
 *  ハブ内蔵IMUはハブの内部にあるIMUで、ハブの傾きや加速度を検知できる
 *  実体はSpikeの筐体に内蔵されている
 *
 * HACK: 
 *  2025の大会では、このデバイスを使用しない
 *  ロボットに対して斜めに取り付けられており、ロボットの水平方向、鉛直方向への換算が必要で、
 *  非常に難易度が高かった。
 *  また測定精度にも疑問がある。過去の大会でこのデバイスを活用して、成績を上げたチームがいなかった。
 *  有効活用すれば、ロボットの自己位置推定を強化し、タスク遂行率を高めることができそうだが・・・
 */
extern IMU imu;

#endif