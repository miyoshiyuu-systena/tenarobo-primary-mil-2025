#ifndef TARGET_IN_IMAGE_CLOSER_H
#define TARGET_IN_IMAGE_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"
#include "ImageAnalysisServer.h"

/**
 * 対象物が画像の視野内に入った時に停止する
 * @return 対象物が画像の視野内に入った時に停止する
 */ 
ICloserGenerator targetInImageCloserGenerator(enum AnalysisCommand command);

/**
 * 対象物が画像の視野内に入った時に停止する
 */ 
class TargetInImageCloser : public ICloser
{
    public:
        TargetInImageCloser(Device*& device, enum AnalysisCommand command);
        ~TargetInImageCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 分析コマンド
         * レスポンスとして画像上にあるかどうかboolを返すものでなければうまく働かない
         */
        enum AnalysisCommand mCommand;

        /**
         * 対象物が画像の視野内に入ったかどうかを返す関数
         */
        IsInImageCallback mIsInImageCallback;
};
#endif