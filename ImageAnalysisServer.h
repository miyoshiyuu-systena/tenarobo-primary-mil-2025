#ifndef IMAGE_ANALYSIS_SERVER_H
#define IMAGE_ANALYSIS_SERVER_H

#include <cstdint>

const uint16_t IMAGE_WIDTH = 320;
const uint16_t IMAGE_HEIGHT = 240;

/**
 * 座標を受け取るコールバック関数の型
 */
typedef void (*CoordinateCallback)(uint16_t& x, uint16_t& y);

/**
 * 対象物が画像の視野内に入ったかどうかを返すコールバック関数の型
 */
typedef bool (*IsInImageCallback)();

/**
 * 分析コマンド
 * Python画像分析サーバーとの間で番号と意味の合意を維持し続けなければならない
 */
enum AnalysisCommand {
    DO_NOTHING = 0,
    FRONT_STRAIGHT = 1,
    TARGET_CIRCLE_IN_DISPLAY = 2,
    ON_LEFT_EDGE = 3,
    GATE_IN_FRONT = 4,
    RED_BOTTLE_IN_FRONT = 5,
    BLUE_BOTTLE_IN_FRONT = 6,
    TARGET_CIRCLE_XY = 7,
    BLUE_BOTTLE_XY = 8,
    ANALYSIS_COMMAND_MAX = 9
};

/**
 * 分析結果パケット
 * Python画像分析サーバーとの間で番号と意味の合意を維持し続けなければならない
 */
struct AnalysisResultPacket {
    uint8_t front_straight : 1;
    uint8_t target_circle_in_display : 1;
    uint8_t on_left_edge : 1;
    uint8_t gate_in_front : 1;
    uint8_t red_bottle_in_front : 1;
    uint8_t blue_bottle_in_front : 1;
    uint8_t reserved6 : 1; //今は使用していない
    uint8_t reserved7 : 1; //今は使用していない
    uint16_t target_circle_x;
    uint16_t target_circle_y;
    uint16_t blue_bottle_x;
    uint16_t blue_bottle_y;
};

/**
 * 画像分析サーバー
 * 
 * @note
 *  TOPPERS/ASP3の中でOpenCVを使用して画像処理すると
 *  CPU重複ロックによる謎の実行エラーが発生する
 *  そのため、画像分析はraspike-artの外に仮想サーバーを作成し、そこに委託する
 *  このクラスは、仮想サーバーのインターフェースを提供する。
 */
class ImageAnalysisServer {
public:
    /**
     * シングルトンインスタンス
     */
    static ImageAnalysisServer& getInstance();

    /**
     * 終了時にメモリを開放する
     */
    void cleanup();

    /**
     * 画像分析をしないように依頼する
     * @note
     *  このメソッドを実行すると次の指示があるまで
     *  画像処理や値の更新を実施しない
     */
    void requestDoNothing();

    /**
     * 画像分析を依頼する
     * @param command 分析コマンド
     */
    void request(enum AnalysisCommand command);

    /**
     * 直線が検出されたかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 直線が検出されたかどうか
     */
    bool responseFrontStraight();
    static bool responseFrontStraightStatic();
    /**
     * 画面内にターゲットサークルがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 画面内にターゲットサークルがあるかどうか
     */
    bool responseTargetCircleInDisplay();
    static bool responseTargetCircleInDisplayStatic();

    /**
     * 左端に乗っているかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 左端に乗っているかどうか
     */
    bool responseOnLeftEdge();
    static bool responseOnLeftEdgeStatic();
    /**
     * 正面にゲートがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return ゲートがあるかどうか
     */
    bool responseGateInFront();
    static bool responseGateInFrontStatic();

    /**
     * 正面に赤いペットボトルがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 赤いペットボトルがあるかどうか
     */
    bool responseRedBottleInFront();
    static bool responseRedBottleInFrontStatic();

    /**
     * 正面に青いペットボトルがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 青いペットボトルがあるかどうか
     */
    bool responseBlueBottleInFront();
    static bool responseBlueBottleInFrontStatic();

    /**
     * ターゲットサークルの座標を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return ターゲットサークルの座標
     */
    void responseTargetCircleXY(uint16_t& x, uint16_t& y);
    static void responseTargetCircleXYStatic(uint16_t& x, uint16_t& y);

    /**
     * 青いペットボトルの座標を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 青いペットボトルの座標
     */
    void responseBlueBottleXY(uint16_t& x, uint16_t& y);
    static void responseBlueBottleXYStatic(uint16_t& x, uint16_t& y);

private:
    ImageAnalysisServer();
    ~ImageAnalysisServer();
};

#endif // IMAGE_ANALYSIS_SERVER_H