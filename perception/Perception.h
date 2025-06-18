#ifndef _PERCEPTION_H_
#define _PERCEPTION_H_

#include <cstdint>

/**
 * 知覚データを管理するクラス
 * 知覚データはデバイスから取得したデータに基づいて代入され、他の方法で上書きされなし
 * 知覚データの閲覧は、データ間で不整合を起こさないことを保証している
 */
class Perception {
public:
    Perception();

    /**
     * 正面の物体との距離
     * @note
     *  超音波センサーによる測定値
     *  単位: mm
     *  何も検知できていない時は-1を返す
     */
    int getDistance();

    /**
     * @note
     *  HSV値
     *  色空間の一つで、色相(Hue)、彩度(Saturation)、明度(Value)の3つの値で色を表す
     *  兄弟にRGB値がいる。これもカラーセンサで取得できるが、どちらが良いかは要検討
     * 
     * TODO: RGB値での運転の方がいいかもしれないので、試験してみる
     */
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
     * 前腕モーターの回転速度
     * @note
     *  モーターによる測定値
     *  単位: °/s
     *
     * TODO:
     *  前腕の回転速度を取得する必要があるのか未検討
     *  どちらかというとエンコード値を取得した方がいい気がする
     */
    float getFrontArmSpeed();

    /**
     * 正面の画像
     * @note
     *  カメラによる測定値
     *  Webカメラの時間分解能のため、他の値と比較するとスローペースで更新される可能性があり
     *  仕様書を要確認
     *  
     * TODO: カメラの画像を取得する
     */
    int getFrontImage();

    /**
     * データ更新を宣言するマスク
     * 逆に言うと更新する必要のないデータは更新しない
     * 
     * @param mask データ更新を宣言するマスク
     * 
     * @note
     *  0: 更新しない
     *  1: 更新する
     *  
     *  [7] Webカメラ
     *  [6] 超音波センサー
     *  [5] カラーセンサー
     *  [4] 圧力センサー
     *  [3] 左モーター
     *  [2] 右モーター
     *  [1] 前腕モーター
     *  [0] ハブ内蔵IMU
     */
    void setMask(uint8_t mask);

    /**
     * データ更新
     * 
     * @note
     *  データ更新を宣言したマスクに応じて、データを更新する
     */
    void update();

private:
    /**
     * カメラの保存間隔
     */
    static const int cameraSaveInterval = 1000;

    /**
     * 知覚タスクのサイクル回数
     * 測定を間引きするために使用
     */
    int count_cycle;

    /**
     * 取得された知覚データ
     */
    // int mFrontImage;
    int mDistance;
    int mColorH;
    int mColorS;
    int mColorV;
    float mForce;
    float mLeftMotorSpeed;
    float mRightMotorSpeed;
    float mFrontArmSpeed;

    /**
     * データ更新を宣言するマスク
     */
    uint8_t mMask;
    static constexpr uint8_t MASK_CAMERA      = 0b10000000;
    static constexpr uint8_t MASK_ULTRASONIC  = 0b01000000;
    static constexpr uint8_t MASK_COLOR       = 0b00100000;
    static constexpr uint8_t MASK_FORCE       = 0b00010000;
    static constexpr uint8_t MASK_LEFT_MOTOR  = 0b00001000;
    static constexpr uint8_t MASK_RIGHT_MOTOR = 0b00000100;
    static constexpr uint8_t MASK_FRONT_ARM   = 0b00000010;
    static constexpr uint8_t MASK_IMU         = 0b00000001;
};

/**
 * グローバルなインスタンス
 * 実体は Perception.cpp で定義されている
 */
extern Perception perception;

#endif // _PERCEPTION_H_