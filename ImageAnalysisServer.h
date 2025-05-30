#ifndef IMAGE_ANALYSIS_SERVER_H
#define IMAGE_ANALYSIS_SERVER_H

#include <cstdint>

/**
 * 分析コマンド
 * Python画像分析サーバーとの間で番号と意味の合意を維持し続けなければならない
 */
enum AnalysisCommand {
    DO_NOTHING = 0,
    FRONT_STRAIGHT = 1,
    ON_RIGHT_EDGE = 2,
    ON_LEFT_EDGE = 3,
    GATE_IN_FRONT = 4,
    RED_BOTTLE_IN_FRONT = 5,
    BLUE_BOTTLE_IN_FRONT = 6,
    ANALYSIS_COMMAND_MAX = 7
};

/**
 * 分析結果パケット
 * Python画像分析サーバーとの間で番号と意味の合意を維持し続けなければならない
 */
struct AnalysisResultPacket {
    uint8_t front_straight : 1;
    uint8_t on_right_edge : 1;
    uint8_t on_left_edge : 1;
    uint8_t gate_in_front : 1;
    uint8_t red_bottle_in_front : 1;
    uint8_t blue_bottle_in_front : 1;
    uint8_t reserved6 : 1; //今は使用していない
    uint8_t reserved7 : 1; //今は使用していない
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

    /**
     * 右端に乗っているかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 右端に乗っているかどうか
     */
    bool responseOnRightEdge();

    /**
     * 左端に乗っているかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 左端に乗っているかどうか
     */
    bool responseOnLeftEdge();

    /**
     * 正面にゲートがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return ゲートがあるかどうか
     */
    bool responseGateInFront();

    /**
     * 正面に赤いペットボトルがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 赤いペットボトルがあるかどうか
     */
    bool responseRedBottleInFront();

    /**
     * 正面に青いペットボトルがあるかどうか,分析結果を読み取る
     * 事前にrequest(enum AnalysisCommand command)を実行しておく必要がある
     * @return 青いペットボトルがあるかどうか
     */
    bool responseBlueBottleInFront();

private:
    ImageAnalysisServer();
    ~ImageAnalysisServer();
};

#endif // IMAGE_ANALYSIS_SERVER_H