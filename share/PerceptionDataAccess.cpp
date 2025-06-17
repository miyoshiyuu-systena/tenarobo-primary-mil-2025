#include "PerceptionDataAccess.h"

/**
 * 知覚データへのアクセス
 * グローバルデータとしてインスタンス化する
 */
PerceptionDataAccess perceptionDataAccess = {
    .force = 0.0f,
    .distance = 0,
    .color = {0, 0, 0},
    .brightness = 0,
    // .leftMotorRotationCount = 0,
    // .rightMotorRotationCount = 0,
    .leftMotorSpeed = 0,
    .rightMotorSpeed = 0,
    // .leftMotorPower = 0,
    // .rightMotorPower = 0
}; 