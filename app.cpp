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
#include "ChangeDirectionAction.h"
#include "FineChangeDirectionAction.h"
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
#include "GateFrontCloser.h"
#include "TargetInImageCloser.h"
#include "CurveCloser.h"
#include "TimedCloser.h"
#include "OnRightEdgeCloser.h"
#include "ObstacleCloser.h"
#include "ObstacleNearCloser.h"
#include "NoObstacleCloser.h"
#include "RunDistanceCloser.h"
#include "OraOraAction.h"
#include "LaneChangeAssist.h"
#include "ImageAnalysisServer.h"

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

    bool is_right = false;

    ActionNode* root = new ActionNode(
        "action0: 背中のボタンを押すまで忠犬ハチ公！！！",
        &device,
        hachikouActionFactory(
            1.0f,
            10
        ),
        0
    );

    ActionNode* action1 = new ActionNode(
        "action1: 直進する、曲線に差し掛かるまで",
        &device,
        goStraightActionFactory(
            500.0f,//500mm/s
            // 100,//判定間隔100ms
            10,//判定間隔10ms(ペットボトルを検知するならば早い方がいい, 超音波センサは10msに1回センサ値を取得する) // サンプル
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    150.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                // curveCloserGenerator()//曲線に差し掛かったら終了判定を出す（実際には直線を検知できなくなったら終了判定を出す）
                obstacleCloserGenerator(200)//前方150mm以内に障害物を検知したら終了判定を出す // サンプル
            }
        ),
        0
    );
    
    /**
     * TASK
     * 最初の真っ直ぐな走路上にはペットボトルが配置されていることがわかっている
     * そのまままっすぐ行くとロボットが前腕でペットボトルを掴んで運んでしまう可能性がある
     * そのケースでは以下の弊害が考えられる。
     * 1. カメラの手前の方にペットボトルの上部が映り込み、期待通りの画像分析ができない
     * 2. スマートキャリーの時点でペットボトルを抱え込んでいる場合は、キャリー用のボトルを掴むことができず、1つのペットボトルの課題をクリアできない
     * 3. 途中でペットボトルが点灯してロボットの走行を妨げる
     * 
     * なんらかの対処が必要
     */
    // 例えば・・・
    ActionNode* action1_1 = new ActionNode(
        "action1_1: オラオラ！！！ロボット様のお通りだぜ！！！左右に蛇行しながらペットボトルにタックル！！！",
        &device,
        oraOraActionFactory(
            is_right,//最初に右側に首を振る
            {
                noObstacleCloserGenerator(150)//前方150mm以内に障害物がないことを終了判定を出す
            }
        ),
        0
    );

    ActionNode* action1_2 = new ActionNode(
        "action1_2: 直線をゆっくり走行する",
        &device,
        goStraightActionFactory(
            250.0f,//100mm/s
            100,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                curveCloserGenerator()//曲線に差し掛かったら終了判定を出す（実際には直線を検知できなくなったら終了判定を出す）
            }
        ),
        0
    );

    ActionNode* action2 = new ActionNode(
        "action2: 曲線を走行する、直線に差し掛かるまで",
        &device,
        goCurveActionFactory(
            150.0f,//150mm/s
            150,//半径150mm
            is_right,//右方向に曲がる
            100,//判定時間100ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                straightCloserGenerator()//直線を検知したら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action3 = new ActionNode(
        "action3: 直進する、曲線に差し掛かるまで",
        &device,
        goStraightActionFactory(
            500.0f,//500mm/s
            100,//判定間隔100ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                ),
                slowlyAccelerateAssistGenerator(//I制御の効果を高めるためにゆっくり加速する
                    10,//10段階加速する, 500 / 10 = 50mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                )
            },
            {
                curveCloserGenerator()//曲線に差し掛かったら終了判定を出す（実際には直線を検知できなくなったら終了判定を出す）
            }
        ),
        0
    );

    ActionNode* action4 = new ActionNode(
        "action4: 曲線を走行する、直線に差し掛かるまで",
        &device,
        goCurveActionFactory(
            150.0f,//150mm/s
            150,//半径150mm
            is_right,//右に曲がる
            100,//判定時間100ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                straightCloserGenerator()//直線を検知したら終了判定を出す
            }
        ),
        0
    );

    root->setNext(action1);
    action1->setNext(action1_1);
    action1_1->setNext(action1_2);
    action1_2->setNext(action2);
    action2->setNext(action3);
    action3->setNext(action4);

    /**
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$ここからダブルループ$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     */

    ActionNode* action5 = new ActionNode(
        "action5: 直進する、青い床に差し掛かるまで",
        &device,
        goStraightActionFactory(
            250.0f,//250mm/s（ダブルループのエントリーは大事にしたい、低速で侵入する）
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                )
            },
            {
                blueFloorCloserGenerator()//青い線に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action6 = new ActionNode(
        "action6: 青い床を直進する、黒い床に差し掛かるまで",
        &device,
        goStraightActionFactory(
            250.0f,//250mm/s
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlueWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                )
            },
            {
                blackFloorCloserGenerator()//黒い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action7 = new ActionNode(
        "action7: 黒い線に沿って旋回する、青い床に差し掛かるまで(大ループ)",
        &device,
        goCurveActionFactory(//旋回する
            250.0f,//速度250mm/s
            400.0f,//半径400mm,
            is_right,//右に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                ),
                slowlyAccelerateAssistGenerator(//I制御の効果を高めるためにゆっくり加速する
                    10,//10段階加速する, 250 / 10 = 25mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                ),
                laneChangeAssistGenerator(is_right)     //右車線に車線変更する
                                                        //これ必要？？？
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action8 = new ActionNode(
        "action8: 青い線に沿って旋回する、黒い床に差し掛かるまで",
        &device,
        goCurveActionFactory(//旋回する
            250.0f,//速度250mm/s
            400.0f,//半径400mm,
            is_right,//右に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlueWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                )
            },
            {
                blackFloorCloserGenerator()//黒い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action9 = new ActionNode(
        "action9: 黒い線に沿って旋回する、青い床に差し掛かるまで(小ループ)",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s, 小ループはゆっくり走行しないと、カーブが急すぎて線を逸脱しやすい
            180.0f,//半径180mm
            !is_right,//左に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    !is_right,//線の左縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                ),
                slowlyAccelerateAssistGenerator(//I制御の効果を高めるためにゆっくり加速する
                    10,//10段階加速する, 150 / 10 = 15mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                ),
                laneChangeAssistGenerator(!is_right)//左車線に車線変更する
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action10 = new ActionNode(
        "action10: 青い線に沿って旋回する、黒い床に差し掛かるまで",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s, 小ループはゆっくり走行しないと、カーブが急すぎて線を逸脱しやすい
            180.0f,//半径180mm
            !is_right,//左に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    !is_right,//線の左縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlueWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                )
            },
            {
                blackFloorCloserGenerator()//黒い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action11 = new ActionNode(
        "action11: 黒い線に沿って旋回する、青い床に差し掛かるまで(大ループ)",
        &device,
        goCurveActionFactory(
            250.0f,//速度250mm/s
            400.0f,//半径400mm
            is_right,//右に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(黒い線と白い線の境界を活用する)
                ),
                slowlyAccelerateAssistGenerator(//I制御の効果を高めるためにゆっくり加速する
                    10,//10段階加速する, 250 / 10 = 25mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                ),
                laneChangeAssistGenerator(is_right)//右車線に車線変更する
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action12 = new ActionNode(
        "action12: 青い線に沿って旋回する、黒い床に差し掛かるまで",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s
            400.0f,//半径400mm
            is_right,//右に旋回する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlueWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                )
            },
            {
                blackFloorCloserGenerator()//黒い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action13 = new ActionNode(
        "action13: 直進する, 青い床に差し掛かるまで",
        &device,
        goStraightActionFactory(
            150.0f,//速度150mm/s //この間にペットボトルを抱え込むために低速で走行する
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    !is_right,//線の左縁にそう
                    100.0f,//比例ゲイン
                    0.1f,//積分ゲイン
                    10.0f,//微分ゲイン
                    calcBlueWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                ),
                slowlyAccelerateAssistGenerator(//I制御の効果を高めるためにゆっくり加速する
                    10,//10段階加速する, 150 / 10 = 15mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                ),
                laneChangeAssistGenerator(!is_right)//左車線に車線変更する
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    action4->setNext(action5);
    action5->setNext(action6);
    action6->setNext(action7);
    action7->setNext(action8);
    action8->setNext(action9);
    action9->setNext(action10);
    action10->setNext(action11);
    action11->setNext(action12);
    action12->setNext(action13);


    /**
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$ここからスマートキャリー$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     * $$$$$$$$$$$$$$$$$$$$$$$$$$
     */

    ActionNode* action14 = new ActionNode(
       "action14: 前進する、一定の距離だけ(ゲートを検知しやすい位置まで移動する)",
       &device,
       goStraightActionFactory(
           150.0f,//速度150mm/s
           10,//判定間隔10ms
           {},
           {
               runDistanceCloserGenerator(760.0f)//760mm走行したら終了判定を出す
           }
       ),
       0
    );

    ActionNode* action15 = new ActionNode(
       "action15: ゲートを探してその場で回転する",
       &device,
       changeDirectionActionFactory(
           is_right,
           {
               gateFrontCloserGenerator()//ゲートを検知したら終了判定を出す
           }
       ),
       0
    );

    ActionNode* action16 = new ActionNode(
       "action16: 直進する、一定の距離だけ",
       &device,
       goStraightActionFactory(
           500.0f,//速度500mm/s
           10,//判定間隔10ms
           {},
           {
               runDistanceCloserGenerator(1000.0f)     //1000mm走行したら終了判定を出す
                                                       /**
                                                        * TASK
                                                        * ゲートを通過するまでの走行の終了条件は洗練したい
                                                        */
           }
       ),
       0
    );

    ActionNode* action17 = new ActionNode(
       "action17: ちゃんと停止する",
       &device,
       stopActionFactory(),
       0
    );

    ActionNode* action18 = new ActionNode(
       "action18: ターゲットサークルを探してその場で回転する",
       &device,
       changeDirectionActionFactory(
           is_right,
                       /**
                        * TASK
                        * ゲートを通過した直後に右手にターゲットサークルがあるとは限らない
                        * 左右両方を確認できるようにしたい
                        */
           {
                targetInImageCloserGenerator(TARGET_CIRCLE_IN_DISPLAY)//ターゲットサークルを検知したら終了判定を出す
           }
       ),
       0
    );

    ActionNode* action18_1 = new ActionNode(
        "action18_1: ターゲットサークルを探してその場で回転する(細かく)",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action18_2 = new ActionNode(
        "action18_2: ターゲットサークルとの距離が760mm以内になるまで前進する",
        &device,
        goStraightActionFactory(
            100.0f,//速度100mm/s
            100,//判定間隔100ms
            {},
            {
                obstacleNearCloserGenerator(TARGET_CIRCLE_XY)//ターゲットサークルとの距離が760mm以内になるまで前進する
            }
        ),
        0
    );

    ActionNode* action18_3 = new ActionNode(
        "action18_3: ターゲットサークルを探してその場で回転する(細かく)",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action19 = new ActionNode(
       "action19: 直進する、一定の距離だけ",
       &device,
       goStraightActionFactory(
           500.0f,//速度500mm/s
           10,//判定間隔10ms
           {},
           {
               runDistanceCloserGenerator(760.0f)     //760mm走行したら終了判定を出す
                                                       /**
                                                        * TASK
                                                        * ターゲットサークルまでの距離が760mmだとは限らない
                                                        * 画像の中心にターゲットサークルが差し掛かるまで、ゆっくり前進するアクションを追加する
                                                        */
           }
       ),
       0
    );

    ActionNode* action20 = new ActionNode(
       "action20: 後ろに一定の距離だけ進む",
       &device,
       goStraightActionFactory(
           -500.0f,    //速度-500mm/s後ろ向き
                       /**
                        * XXX
                        * これが後ろ向き走行で機能するかわからない
                        * というより多分機能しない
                        */
           10,//判定間隔10ms
           {},
           {
               runDistanceCloserGenerator(760.0f)     //760mm走行したら終了判定を出す
                                                       /**
                                                        * XXX
                                                        * これが後ろ向き走行で機能するかわからない
                                                        * というより多分機能しない
                                                        */
           }
       ),
       0
    );

    ActionNode* action21 = new ActionNode(
       "action21: 青いペットボトルを探して、その場で回転する",
       &device,
       changeDirectionActionFactory(
           !is_right,//左に回転する
           {
               targetInImageCloserGenerator(BLUE_BOTTLE_IN_FRONT)//青いペットボトルを検知したら終了判定を出す
           }
       ),
       0
    );

    ActionNode* action22 = new ActionNode(
       "action22: 青いペットボトルを探して、その場で回転する(細かく)",
       &device,
       fineChangeDirectionActionFactory(BLUE_BOTTLE_XY),
       0
    );

    ActionNode* action23 = new ActionNode(
        "action23: ターゲットサークルを探してその場で回転する",
        &device,
        changeDirectionActionFactory(
            !is_right,
                        /**
                         * TASK
                         * ゲートを通過した直後に右手にターゲットサークルがあるとは限らない
                         * 左右両方を確認できるようにしたい
                         */
            {
                 targetInImageCloserGenerator(TARGET_CIRCLE_IN_DISPLAY)//ターゲットサークルを検知したら終了判定を出す
            }
        ),
        0
     );

    ActionNode* action24 = new ActionNode(
        "action24: ターゲットサークルを探してその場で回転する(細かく)",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action25 = new ActionNode(
        "action25: ターゲットサークルとの距離が760mm以内になるまで前進する",
        &device,
        goStraightActionFactory(
            100.0f,//速度100mm/s
            100,//判定間隔100ms
            {},
            {
                obstacleNearCloserGenerator(TARGET_CIRCLE_XY)//ターゲットサークルとの距離が760mm以内になるまで前進する
            }
        ),
        0
    );

    ActionNode* action26 = new ActionNode(
        "action26: ターゲットサークルを探してその場で回転する(細かく)",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action27 = new ActionNode(
        "action27: 直進する、一定の距離だけ",
        &device,
        goStraightActionFactory(
            500.0f,//速度500mm/s
            10,//判定間隔10ms
            {},
            {
                runDistanceCloserGenerator(760.0f)     //760mm走行したら終了判定を出す
                                                        /**
                                                         * TASK
                                                         * ターゲットサークルまでの距離が760mmだとは限らない
                                                         * 画像の中心にターゲットサークルが差し掛かるまで、ゆっくり前進するアクションを追加する
                                                         */
            }
        ),
        0
     );

     ActionNode* action28 = new ActionNode(
        "action28: 後ろに一定の距離だけ進む",
        &device,
        goStraightActionFactory(
            -500.0f,    //速度-500mm/s後ろ向き
                        /**
                         * XXX
                         * これが後ろ向き走行で機能するかわからない
                         * というより多分機能しない
                         */
            10,//判定間隔10ms
            {},
            {
                runDistanceCloserGenerator(760.0f)     //760mm走行したら終了判定を出す
                                                        /**
                                                         * XXX
                                                         * これが後ろ向き走行で機能するかわからない
                                                         * というより多分機能しない
                                                         */
            }
        ),
        0
     );

    action13->setNext(action14);
    action14->setNext(action15);
    action15->setNext(action16);
    action16->setNext(action17);
    action17->setNext(action18);
    action18->setNext(action18_1);
    action18_1->setNext(action18_2);
    action18_2->setNext(action18_3);
    action18_3->setNext(action19);
    action19->setNext(action20);
    action20->setNext(action21);
    action21->setNext(action22);
    action22->setNext(action23);
    action23->setNext(action24);
    action24->setNext(action25);
    action25->setNext(action26);
    action26->setNext(action27);
    action27->setNext(action28);

    ActionNode* current = root;
    ActionNode* next = nullptr;

    do {
        current->execute();
        next = current->getNext();
        delete current;
        current = next;
    } while (current != nullptr);

    // 最終的なログファイル書き込み
    logger.writeLogsToFile();
    logger.logInfo("ActionChainサンプルプログラム終了");
    
    //  タスク終了
    ext_tsk();
}