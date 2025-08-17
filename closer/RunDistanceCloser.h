#ifndef RUN_DISTANCE_CLOSER_H
#define RUN_DISTANCE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"

/**
 * 走行距離が一定以上になったら終了判定を出すクラスのファクトリー関数
 * @param distance 走行距離(mm)
 * @return 走行距離が一定以上になったら終了判定を出すクラスのファクトリー関数
 */
ICloserGenerator runDistanceCloserGenerator(float distance);

/**
 * 走行距離が一定以上になったら終了判定を出すクラス
 * 
 */
class RunDistanceCloser : public ICloser
{
    public:
        /**
         * @param device デバイスの参照
         * @param distance 走行距離(mm)
         */
        RunDistanceCloser(Device*& device, float distance);
        ~RunDistanceCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * エンコード値の補正値
         * エンコード値が実際の走行距離よりも大きくなるので、補正値をかけて実際の走行距離に合わせる
         * 
         * > 補正値の求め方
         * 一度補正値1.0fで、決められた距離(500mm)を走行し、実際の走行距離をメジャーで測り（Dmm)、
         * 補正値 = 決められた距離(500mm) / 実際の走行距離(Dmm)
         */
        /**
         * エンコード値の補正値を取得
         * @return エンコード値の補正値
         */
        float getEncodeCorrect() const;
        
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 目標のエンコード値
         */
        int mTargetEncode;

        /**
         * スタート時のエンコード(左)
         */
        int mStartEncodeLeft;

        /**
         * スタート時のエンコード(右)
         */
        int mStartEncodeRight;
};

#endif