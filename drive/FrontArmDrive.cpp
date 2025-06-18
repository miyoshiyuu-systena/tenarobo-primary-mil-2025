#include "FrontArmDrive.h"

FrontArmDrive::FrontArmDrive(EPort port)
    : mMotor(port, Motor::EDirection::COUNTERCLOCKWISE, true)
{
}

FrontArmDrive::~FrontArmDrive()
{
}