#include "ObstacleNearCloser.h"
#include "Logger.h"
#include "spikeapi.h"

ICloserGenerator obstacleNearCloserGenerator(enum AnalysisCommand command) {
    return [command](Device*& device) {
        return new ObstacleNearCloser(device, command);
    };
}

ObstacleNearCloser::ObstacleNearCloser(Device*& device, enum AnalysisCommand command)
: ICloser()
, mDevice(device)
, mCommand(command)
{
}

ObstacleNearCloser::~ObstacleNearCloser()
{
}

void ObstacleNearCloser::init()
{
    mSeqCountLost = 0;

    /**
     * 分析コマンドに応じて、座標を取得するコールバック関数を設定する
     * XXX リクエストとレスポンスをうまく組み合わせることができておらずswitchを使うハメに
     * XXX 要リファクタリング（どっちかというとImageAnalysisServerの方を工夫した方がいいかも）
     */
    switch (mCommand) {
        case BLUE_BOTTLE_XY:
        case TARGET_CIRCLE_XY:
            ImageAnalysisServer::getInstance().request(mCommand);
            mCoordinateCallback = ImageAnalysisServer::responseTargetCircleXYStatic;
            break;
        default:
            return;
    }
    dly_tsk(100 * 1000); // 100ms待機して画像分析タスクの切り替わりを待つ
}

bool ObstacleNearCloser::isClosed()
{

    if (mCoordinateCallback == nullptr) {
        Logger::getInstance().logWarning("正しく画像分析タスクが実行されていません。");
        return true;
    }

    uint16_t x = 0;
    uint16_t y = 0;

    mCoordinateCallback(x, y);

    
    /**
    * 5回以上連続して0, 0のときは
    * もう対象を見失ってしまっていると判断して停止する
    * XXX 本来であれば、周辺をめぐって、もう一度見つけようとするのがいい
    */
    if (x == 0 && y == 0) {
        mSeqCountLost++;

        if (mSeqCountLost > 5) {
            Logger::getInstance().logWarning("画像から対象物を連続して5回見失ったため、アクションを終了します。");
            return true;
        } else {
            return false;
        }
    }

    mSeqCountLost = 0;

    return y > (uint16_t)(IMAGE_HEIGHT / 2);
}