#ifndef OBSTACLE_CLOSER_H
#define OBSTACLE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"

/**
 * 障害物があることを終了判定を出すクラスのファクトリー関数
 * @param detectDistance 障害物検知距離（これよりも近いところは障害物ありと判定する）
 * @return 障害物があることを終了判定を出すクラス
 */
ICloserGenerator obstacleCloserGenerator(int detectDistance);

class ObstacleCloser : public ICloser
{
    public:
        ObstacleCloser(Device*& device, int detectDistance);
        ~ObstacleCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 障害物検知距離
         */
        int mDetectDistance;
};

#endif