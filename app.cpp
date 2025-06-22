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
    cv::Mat imageB = cv::imread("/home/mil/work/RasPike-ART/sdk/workspace/img/38_知覚データを取得する2.jpg");

    cv::Mat image_blurA;
    cv::GaussianBlur(imageA, image_blurA, cv::Size(5, 101), 0);
    CameraManager::getInstance().saveImage(image_blurA);
    cv::Mat image_blurB;
    cv::GaussianBlur(imageB, image_blurB, cv::Size(5, 101), 0);
    CameraManager::getInstance().saveImage(image_blurB);

    // 灰色検出の実装
    // 灰色っぽい色の範囲を定義（RGB値が近しい値）
    // 下限: 少し暗めの灰色 (R,G,Bの差が小さく、全体的に暗め)
    cv::Scalar lower_gray = cv::Scalar(60, 60, 60);   // BGR順
    // 上限: 少し明るめの灰色 (R,G,Bの差が小さく、全体的に明るめ)
    cv::Scalar upper_gray = cv::Scalar(160, 160, 160); // BGR順
    
    // より厳密な灰色検出のため、RGB値の差も考慮
    cv::Mat gray_maskA, gray_maskB;
    cv::Mat gray_resultA, gray_resultB;
    
    // 画像Aの灰色検出
    cv::inRange(image_blurA, lower_gray, upper_gray, gray_maskA);
    
    // さらに厳密にするため、RGB値の差が小さいかチェック
    cv::Mat channels_A[3];
    cv::split(image_blurA, channels_A);
    cv::Mat diff_AB_A, diff_BC_A, diff_AC_A;
    cv::absdiff(channels_A[0], channels_A[1], diff_AB_A);  // B-G
    cv::absdiff(channels_A[1], channels_A[2], diff_BC_A);  // G-R  
    cv::absdiff(channels_A[0], channels_A[2], diff_AC_A);  // B-R
    
    // RGB値の差が30以下の部分を抽出
    cv::Mat rgb_diff_maskA;
    cv::Mat temp1_A, temp2_A;
    cv::threshold(diff_AB_A, temp1_A, 30, 255, cv::THRESH_BINARY_INV);
    cv::threshold(diff_BC_A, temp2_A, 30, 255, cv::THRESH_BINARY_INV);
    cv::bitwise_and(temp1_A, temp2_A, rgb_diff_maskA);
    cv::threshold(diff_AC_A, temp1_A, 30, 255, cv::THRESH_BINARY_INV);
    cv::bitwise_and(rgb_diff_maskA, temp1_A, rgb_diff_maskA);
    
    // 基本的な範囲チェックとRGB差チェックを組み合わせ
    cv::bitwise_and(gray_maskA, rgb_diff_maskA, gray_maskA);
    
    // 結果画像を作成（元画像に灰色検出結果を重ねて表示）
    image_blurA.copyTo(gray_resultA);
    gray_resultA.setTo(cv::Scalar(0, 255, 255), gray_maskA); // 検出された灰色部分を黄色でハイライト
    CameraManager::getInstance().saveImage(gray_resultA);
    
    // 画像Bの灰色検出
    cv::inRange(image_blurB, lower_gray, upper_gray, gray_maskB);
    
    cv::Mat channels_B[3];
    cv::split(image_blurB, channels_B);
    cv::Mat diff_AB_B, diff_BC_B, diff_AC_B;
    cv::absdiff(channels_B[0], channels_B[1], diff_AB_B);  // B-G
    cv::absdiff(channels_B[1], channels_B[2], diff_BC_B);  // G-R  
    cv::absdiff(channels_B[0], channels_B[2], diff_AC_B);  // B-R
    
    cv::Mat rgb_diff_maskB;
    cv::Mat temp1_B, temp2_B;
    cv::threshold(diff_AB_B, temp1_B, 30, 255, cv::THRESH_BINARY_INV);
    cv::threshold(diff_BC_B, temp2_B, 30, 255, cv::THRESH_BINARY_INV);
    cv::bitwise_and(temp1_B, temp2_B, rgb_diff_maskB);
    cv::threshold(diff_AC_B, temp1_B, 30, 255, cv::THRESH_BINARY_INV);
    cv::bitwise_and(rgb_diff_maskB, temp1_B, rgb_diff_maskB);
    
    cv::bitwise_and(gray_maskB, rgb_diff_maskB, gray_maskB);
    
    image_blurB.copyTo(gray_resultB);
    gray_resultB.setTo(cv::Scalar(0, 255, 255), gray_maskB); // 検出された灰色部分を黄色でハイライト
    CameraManager::getInstance().saveImage(gray_resultB);
    
    std::cout << "灰色検出処理が完了しました。検出結果は黄色でハイライトされています。" << std::endl;

    // カメラマネージャの終了
    CameraManager::getInstance().shutdownCamera();

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}