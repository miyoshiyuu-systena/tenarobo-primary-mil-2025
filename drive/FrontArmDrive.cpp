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

int FrontArmDrive::getCount() const
{
    return mMotor.getCount();
}

void FrontArmDrive::resetCount()
{
    mMotor.resetCount();
}

void FrontArmDrive::hold()
{
    mMotor.hold();
}

bool FrontArmDrive::isStalled() const
{
    return mMotor.isStalled();
}