#include "PerceptionData.h"

// static int mFrontImage = -1;
static int mDistance = -1;
static int mColorH = -1;
static int mColorS = -1;
static int mColorV = -1;
static float mForce = -1.0f;
static float mLeftMotorSpeed = -1.0f;
static float mRightMotorSpeed = -1.0f;

// int getFrontImage()
// {
//     return mFrontImage;
// }

int getDistance()
{
    return mDistance;
}

int getColorH()
{
    return mColorH;
}

int getColorS()
{
    return mColorS;
}

int getColorV()
{
    return mColorV;
}

float getForce()
{
    return mForce;
}

float getLeftMotorSpeed()
{
    return mLeftMotorSpeed;
}

float getRightMotorSpeed()
{
    return mRightMotorSpeed;
}