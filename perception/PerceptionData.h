#ifndef _PERCEPTION_DATA_H_
#define _PERCEPTION_DATA_H_

/**
 * 正面の画像
 * @note
 *  カメラによる測定値
 *  Webカメラの時間分解能のため、他の値と比較するとスローペースで更新される可能性があり
 *  仕様書を要確認
 *  
 * TODO: カメラの画像を取得する
 */
// int getFrontImage();

/**
 * 正面の物体との距離
 * @note
 *  超音波センサーによる測定値
 *  単位: mm
 *  何も検知できていない時は-1を返す
 */
int getDistance();

/**
 * 床面のHSV値のうちH(色相)の値
 * @note
 *  カラーセンサーによる測定値
 *  単位: 0~360
 */
int getColorH();

/**
 * 床面のHSV値のうちS(彩度)の値
 * @note
 *  カラーセンサーによる測定値
 *  単位: 0~100
 */
int getColorS();

/**
 * 床面のHSV値のうちV(明度)の値
 * @note
 *  カラーセンサーによる測定値
 *  単位: 0~100
 */
int getColorV();

/**
 * 背中のボタンに加えられた力の大きさ
 * @note
 *  圧力センサーによる測定値
 *  単位: N
 */
float getForce();

/**
 * 左モーターの回転速度
 * @note
 *  モーターによる測定値
 *  単位: °/s
 */
float getLeftMotorSpeed();

/**
 * 右モーターの回転速度
 * @note
 *  モーターによる測定値
 *  単位: °/s
 */
float getRightMotorSpeed();


/**
 * 
 * @note 
 *  カラーセンサの床面に対する角度は前腕の角度と連動しているため
 *  必ずしも床面から鉛直方向の反射光を取得できているわけではない
 *
 * @note
 *  HSV値
 *  色空間の一つで、色相(Hue)、彩度(Saturation)、明度(Value)の3つの値で色を表す
 *  兄弟にRGB値がいる。これもカラーセンサで取得できるが、どちらが良いかは要検討
 * 
 * TODO: RGB値での運転の方がいいかもしれないので、試験してみる
 */

#endif