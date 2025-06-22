#include "app.h"
#include "spikeapi.h"
#include "config.h"
#include "Logger.h"
#include "ActionNode.h"
#include "HachikouAction.h"
#include "GoStraightAction.h"
#include "GoCurveAction.h"
#include "PivotTurnAction.h"
#include "SpinTurnAction.h"
#include "LaneChangeAction.h"
#include "SinpleLaneChangeAction.h"
#include "StopAction.h"
#include "Device.h"
#include "LaneTracingAssist.h"
#include "CalcBlackWhiteBorderError.h"
#include "CalcBlueWhiteBorderError.h"
#include "IAssistGenerator.h"
#include "SlowlyAccelerateAssist.h"
#include "BlueFloorCloser.h"
#include "BlackFloorCloser.h"
#include "StraightCloser.h"
#include "StraightStrictCloser.h"
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "CameraManager.h"
#include "PerceptionReporter.h"

using namespace spikeapi;

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void main_task(intptr_t exinf)   {
    // 設定ファイルの読み込みと設定情報のログ出力
    std::cout << "=== メインタスク開始 ===" << std::endl;
    
    if (config.loadConfig()) {
        std::cout << "設定ファイルの読み込みに成功しました" << std::endl;
        config.printConfig();  // 設定情報をコンソールに出力
    } else {
        std::cout << "設定ファイルの読み込みに失敗しました。診断情報を表示します。" << std::endl;
        config.printDiagnostics();  // 診断情報を表示
        std::cout << "デフォルト値を使用して続行します。" << std::endl;
        config.printConfig();  // デフォルト値での設定情報を表示
    }

    // ロガーインスタンスの取得
    Logger& logger = Logger::getInstance();
    
    // カメラマネージャの起動
    CameraManager::getInstance().initializeCamera();

    cv::Mat imageA = cv::imread("/home/mil/work/RasPike-ART/sdk/workspace/img/29_知覚データを取得する2.jpg");

    cv::Mat image_blurA;
    cv::GaussianBlur(imageA, image_blurA, cv::Size(5, 101), 0);
    CameraManager::getInstance().saveImage(image_blurA);

    cv::Mat image_inrangeA;
    cv::inRange(image_blurA, cv::Scalar(0, 0, 0), cv::Scalar(100, 100, 100), image_inrangeA);
    CameraManager::getInstance().saveImage(image_inrangeA);

    /**
     * 画像の上から1 / 3のラインをスライス
     */
    uchar* row_ptr = image_inrangeA.ptr<uchar>(image_inrangeA.rows / 3);
    for (int i = 0; i < image_inrangeA.cols; i++) {
        if (row_ptr[i] == 255) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;

    // カメラマネージャの終了
    CameraManager::getInstance().shutdownCamera();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}