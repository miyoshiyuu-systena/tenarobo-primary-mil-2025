#include "IAssist.h"

IAssist::IAssist(TwinWheelDrive* twinWheelDrive, FrontArmDrive* frontArmDrive, Perception* perc)
    : mTwinWheelDrive(twinWheelDrive)
    , mFrontArmDrive(frontArmDrive)
    , mPerc(perc)
{
}