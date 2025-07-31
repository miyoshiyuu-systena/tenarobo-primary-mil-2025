#ifndef _PERCEPTION_REPORT_H_
#define _PERCEPTION_REPORT_H_

#include <opencv2/opencv.hpp>
#include "Device.h"

/**
 * 知覚制限マスク
 * 
 * @note
 *  1byteのビットごとにデータの種類を割り当てる
 *  bit=1のとき、そのデータを更新する
 * 
 *  知覚を制限するモチベーションは、必要のないデータの取得にかかる時間を削減して、処理全体の遅延を避けること
 */
#define     PERCEPTION_REPORT_MASK_ULTRASONIC   0b10000000
#define     PERCEPTION_REPORT_MASK_FORCE        0b01000000
#define     PERCEPTION_REPORT_MASK_COLOR        0b00100000
#define     PERCEPTION_REPORT_MASK_IMAGE        0b00010000


/**
* 観測結果を格納する構造体
*/
typedef struct {
    /**
    * 超音波センサーの距離
    */
    float distance;
    
    /**
    * 力センサーの力
    */
    float force;
    
    /**
    * カラーセンサーの色
    * HSVのH
    */
    int h;
    
    /**
    * カラーセンサーの色
    * HSVのS
    */
    int s;
    
    /**
    * カラーセンサーの色
    * HSVのV
    */
    int v;
    
    /**
    * Webカメラの画像
    */
    cv::Mat image;
} PerceptionReport;

/**
 * カメラの撮影に最適な間隔
 * [ms]
 */
const int cameraInterval = 33;

/**
 * 知覚データを更新して、格納する
 * 
 * @param device デバイスのインスタンス
 * @param report 観測結果を格納する構造体
 * @param detectInterval 検知間隔[ms]
 * @param mask 格納する情報を制限するマスク（必要なデータだけ取得する）
 * 
 * @note
 *  1byteのビットごとにデータの種類を割り当てる
 *  bit=1のとき、そのデータを更新する
 */
void writePerceptionReport(
    Device* device,
    PerceptionReport& report,
    int detectInterval,
    uint8_t mask
);

#endif