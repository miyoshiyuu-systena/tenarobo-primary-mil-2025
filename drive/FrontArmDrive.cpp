#include "FrontArmDrive.h"
#include "Motor.h"

FrontArmDrive::FrontArmDrive(EPort port)
    : mMotor(port, Motor::EDirection::COUNTERCLOCKWISE, true)
{
}

FrontArmDrive::~FrontArmDrive()
{
}