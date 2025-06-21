#ifndef PERCEPTION_MASK_H
#define PERCEPTION_MASK_H

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
 #define     PERCEPTION_REPORT_MASK_MOTOR_ENCODE 0b00001000

#endif // PERCEPTION_MASK_H