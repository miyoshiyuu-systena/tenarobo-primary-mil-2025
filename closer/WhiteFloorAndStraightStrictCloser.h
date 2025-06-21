#ifndef _WHITE_FLOOR_AND_STRAIGHT_STRICT_CLOSER_H_
#define _WHITE_FLOOR_AND_STRAIGHT_STRICT_CLOSER_H_

#include "ICloser.h"
#include "ICloserGenerator.h"

/**
 * 白い床と目前の直線を両方検知したら終了判定を出すクラスのファクトリー関数
 * @return 白い床と目前の直線を両方検知したら終了判定を出すクラスのファクトリー関数
 */
ICloserGenerator whiteFloorAndStraightStrictGenerator();

/**
 * 白い床と目前の直線を両方検知したら終了判定を出すクラス
 */
class WhiteFloorAndStraightStrict : public ICloser
{
    public:
        WhiteFloorAndStraightStrict();
        ~WhiteFloorAndStraightStrict();

        void init() override;
        bool isClosed() override;
};

#endif