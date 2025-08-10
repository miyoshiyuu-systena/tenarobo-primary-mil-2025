#ifndef _PERCEPTION_REPORT_H_
#define _PERCEPTION_REPORT_H_

#include <opencv2/opencv.hpp>

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

    /**
     * 左モーターのエンコード数
     * @note
     *  エンコード数はタイヤの回転角度を表している
     *  おそらく1つ分が1°を表しているが、要検証
     */
    int leftMotorEncode;

    /**
     * 右モーターのエンコード数
     * @note
     *  エンコード数はタイヤの回転角度を表している
     *  おそらく1つ分が1°を表しているが、要検証
     */
    int rightMotorEncode;
} PerceptionReport;

#endif