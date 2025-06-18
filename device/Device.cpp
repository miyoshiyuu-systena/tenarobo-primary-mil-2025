#include "Device.h"

/**
 * Spike経由で制御するデバイスは、ポート番号A~Fを引数に加える
 * デバイスに割り当てるポート番号は、大会ルールで決められているので要注意！！
 */

TwinWheelDrive twinWheelDrive(
    EPort::PORT_B,      // 左モーターのポート
    EPort::PORT_A       // 右モーターのポート
);

Motor frontArm(
    EPort::PORT_C,                  // 前腕モーターのポート
    Motor::EDirection::CLOCKWISE,   // 前腕モーターの方向
    true                            // 前腕モーターのカウンタリセット
);

ForceSensor forceSensor(EPort::PORT_D);

ColorSensor colorSensor(EPort::PORT_E);

UltrasonicSensor ultrasonicSensor(EPort::PORT_F);

// WebCamera webCamera;

IMU imu;
