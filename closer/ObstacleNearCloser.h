#ifndef OBSTACLE_NEAR_CLOSER_H
#define OBSTACLE_NEAR_CLOSER_H

#include "ICloser.h"
#include "ICloserGenerator.h"
#include "Device.h"
#include "ImageAnalysisServer.h"

/**
 * 対象物が近いときに停止する関数のクローザー
 * 厳密にいうと画像の上下中央よりも手前に来たとき
 * @return 対象物の奥行 
 */
ICloserGenerator obstacleNearCloserGenerator(enum AnalysisCommand command);

/**
 * XXX 
 * このクローザーはまだ検証されていない
 * 走行しながらだとうまく判定できないかもしれない
 * とにかくゆっくりと進行すべし
 */
/**
 * 画像の上下中央より手前に来たときに停止する
 */
class ObstacleNearCloser : public ICloser
{
    public:
        ObstacleNearCloser(Device*& device, enum AnalysisCommand command);
        ~ObstacleNearCloser();

        void init() override;
        bool isClosed() override;

    private:
        /**
         * デバイスの参照
         */
        Device*& mDevice;

        /**
         * 分析コマンド
         * レスポンスとして画像上の座標x, yを返すものでなければうまく働かない
         */
        enum AnalysisCommand mCommand;

        /**
         * 座標を返す関数
         */
        CoordinateCallback mCoordinateCallback;

        /**
         * 座標連続見失い期間
         */
        int mSeqCountLost;
};

#endif //OBSTACLE_NEAR_CLOSER_H