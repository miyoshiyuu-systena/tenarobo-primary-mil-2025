#include "PerceptionDataAccess.h"

/**
 * 知覚データへのアクセス
 * グローバルデータとしてインスタンス化する
 */
PerceptionDataAccess perceptionDataAccess = {
    .force = 0.0f,
    .distance = 0,
    .color = {0, 0, 0},
    .leftMotor = 0,
    .rightMotor = 0
}; 