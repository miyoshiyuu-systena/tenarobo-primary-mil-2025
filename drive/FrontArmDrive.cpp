#include "FrontArmDrive.h"

FrontArmDrive::FrontArmDrive(EPort port)
    : mMotor(port, Motor::EDirection::COUNTERCLOCKWISE, true)
{
}

FrontArmDrive::~FrontArmDrive()
{
}

void FrontArmDrive::setSpeed(float speed)
{
    mMotor.setSpeed(speed);
}

float FrontArmDrive::getSpeed() const
{
    return mMotor.getSpeed();
}