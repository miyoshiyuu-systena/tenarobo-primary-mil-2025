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
#include "FineChangeDirectionLineAction.h"
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

    bool is_right = config.isRCourse();

    Logger::getInstance().logInfo("最初のコーナーを黒い線の外縁でライントレース走行するようにロボットを配置してください。");

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
        "action1: 直線走行",
        &device,
        goStraightActionFactory(
            800.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    80.0f,
                    5.0f,
                    20.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                obstacleCloserGenerator(80) // 障害物がロボットの筐体にベッタリくっついている状態のとき距離75mm程度になる
            }
        ),
        0
    );

    ActionNode* action2 = new ActionNode(
        "action2: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action3 = new ActionNode(
        "action3: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            50,
            {
                laneTracingAssistGenerator(
                    is_right,
                    50.0f,
                    20.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                // curveCloserGenerator()
                runDistanceCloserGenerator(850)
            }
        ),
        0
    );

    /**
     * 最初のカーブで脱線する場合は、走行をよく観察し、Kp, Ki, Kdを調整する
     * 直線に回復する際に脱線する場合はrunDistanceCloserGeneratorの距離を長めにする
     */
    ActionNode* action4 = new ActionNode(
        "action4: 曲がる",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    150.0f,
                    50.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(600)    //十分に確保する
                                                    //次のアクションで直線検知クローザーを効果的に作動させるため、カーブを曲がり切って、直線を正面にしないといけない
                                                    //本番はカーブが短い代わりに直線判定が早いため、このくらいがいいのでは？
                                            
            }
        ),
        0
    );

    ActionNode* action5 = new ActionNode(
        "action5: 直進する（PIDを緩和して体を正面に向ける）",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    100.0f,
                    0.0f,
                    100.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(200)
            }
        ),
        0
    );

    ActionNode* action6 = new ActionNode(
        "action6: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action7 = new ActionNode(
        "action7: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            50,
            {
                laneTracingAssistGenerator(
                    is_right,
                    50.0f,
                    20.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                curveCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action8 = new ActionNode(
        "action8: 曲がる",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    150.0f,
                    50.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(600)    //十分に確保する
                                                    //次のアクションで直線検知クローザーを効果的に作動させるため、カーブを曲がり切って、直線を正面にしないといけない
                                                    //本番はカーブが短い代わりに直線判定が早いため、このくらいがいいのでは？
            }
        ),
        0
    );

    ActionNode* action9 = new ActionNode(
        "action9: 直進する（PIDを緩和して体を正面に向ける）",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    100.0f,
                    0.0f,
                    100.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                runDistanceCloserGenerator(200)
            }
        ),
        0
    );

    ActionNode* action10 = new ActionNode(
        "action10: 直進する",
        &device,
        goStraightActionFactory(
            500.0f,
            10,
            {
                laneTracingAssistGenerator(
                    is_right,
                    50.0f,
                    20.0f,
                    50.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                blueFloorCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action11 = new ActionNode(
        "action11: 少しだけ前進する",
        &device,
        goStraightActionFactory(
            500.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(50)
            }
        ),
        0
    );

    ActionNode* action12 = new ActionNode(
        "action12: その場で右90度を向く",
        &device,
        pivotTurnActionFactory(
            90.0f,
            is_right,
            10,
            {
                timedCloserGenerator(100)
            }
        ),
        0
    );

    ActionNode* action13 = new ActionNode(
        "action13: 直進する",
        &device,
        goStraightActionFactory(
            750.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(120)
            }
        ),
        0
    );

    ActionNode* action14 = new ActionNode(
        "action14: 後退する",
        &device,
        goStraightActionFactory(
            -150.0f,
            10,
            {},
            {
                blueFloorCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action15 = new ActionNode(
        "action15: その場で左45度を向く",
        &device,
        pivotTurnActionFactory(
            90.0f,
            !is_right,
            10,
            {
                timedCloserGenerator(30)//ペットボトルが無くなった分帰りの方がよく回る
                                        //その滑りの程度は環境に大きく依存するため、後続の追加回転アクションでカバーする
            }
        ),
        0
    );

    ActionNode* action16 = new ActionNode(
        "action16: その場で左に回転して正面に直線を検知する",
        &device,
        fineChangeDirectionLineActionFactory(!is_right),
        0
    );

    ActionNode* action17 = new ActionNode(
        "action17: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    root->setNext(action1);
    action1->setNext(action2);
    action2->setNext(action3);
    action3->setNext(action4);
    action4->setNext(action5);
    action5->setNext(action6);
    action6->setNext(action7);
    action7->setNext(action8);
    action8->setNext(action9);
    action9->setNext(action10);
    action10->setNext(action11);
    action11->setNext(action12);
    action12->setNext(action13);
    action13->setNext(action14);
    action14->setNext(action15);
    action15->setNext(action16);
    action16->setNext(action17);

    /**
     * ここからダブルループ
     */
    ActionNode* action18 = new ActionNode(
        "action18: 青い床を直進する、黒い床に差し掛かるまで（ダブルループENTRY）",
        &device,
        goStraightActionFactory(
            250.0f,//250mm/s
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    !is_right,//線の左側にそう
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

    ActionNode* action19 = new ActionNode(
        "action19: 黒い線に沿って旋回する、青い床に差し掛かるまで（大ループ）",
        &device,
        goCurveActionFactory(//旋回する
            250.0f,//速度250mm/s
            400.0f,//半径400mm,
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
                    10,//10段階加速する, 250 / 10 = 25mm/sずつ加速する
                    10//判定間隔10ms * 10 = 100msに一回ずつ加速する
                ),
                laneChangeAssistGenerator(!is_right)    //左車線に車線変更する
                                                        //これ必要？？？
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    ActionNode* action20 = new ActionNode(
        "action20: 青い線に沿って旋回する、黒い床に差し掛かるまで（大ループ）",
        &device,
        goCurveActionFactory(//旋回する
            250.0f,//速度250mm/s
            400.0f,//半径400mm,
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

    ActionNode* action21 = new ActionNode(
        "action21: 黒い線に沿って旋回する、青い床に差し掛かるまで（小ループ）",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s, 小ループはゆっくり走行しないと、カーブが急すぎて線を逸脱しやすい
            180.0f,//半径180mm
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
                    10,//10段階加速する, 150 / 10 = 15mm/sずつ加速する
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

    ActionNode* action22 = new ActionNode(
        "action22: 青い線に沿って旋回する、黒い床に差し掛かるまで（小ループ）",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s, 小ループはゆっくり走行しないと、カーブが急すぎて線を逸脱しやすい
            180.0f,//半径180mm
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

    ActionNode* action23 = new ActionNode(
        "action23: 黒い線に沿って旋回する、青い床に差し掛かるまで(大ループ)",
        &device,
        goCurveActionFactory(
            250.0f,//速度250mm/s
            400.0f,//半径400mm
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
                    10,//10段階加速する, 250 / 10 = 25mm/sずつ加速する
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

    ActionNode* action24 = new ActionNode(
        "action24: 青い線に沿って旋回する、黒い床に差し掛かるまで（大ループ）",
        &device,
        goCurveActionFactory(
            150.0f,//速度150mm/s
            400.0f,//半径400mm
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

    ActionNode* action25A = new ActionNode(
        "action25A: 直進する, 青い床に差し掛かるまで（ダブルループESCAPE）、この間にペットボトルを抱え込む",
        &device,
        goStraightActionFactory(
            250.0f,//速度250mm/s
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    150.0f,//比例ゲイン
                    50.0f,//積分ゲイン
                    50.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                ),
                laneChangeAssistGenerator(is_right)//右車線に車線変更する
            },
            {
                runDistanceCloserGenerator(300)
            }
        ),
        0
    );

    ActionNode* action25B = new ActionNode(
        "action25B: 直進する, 青い床に差し掛かるまで（ダブルループESCAPE）、この間にペットボトルを抱え込む",
        &device,
        goStraightActionFactory(
            250.0f,//速度250mm/s
            10,//判定間隔10ms
            {
                laneTracingAssistGenerator(//足元にガイド線がある場合はそれを活用する
                    is_right,//線の右縁にそう
                    100.0f,//比例ゲイン
                    0.0f,//積分ゲイン
                    100.0f,//微分ゲイン
                    calcBlackWhiteBorderError//誤差計算関数(青い線と白い線の境界を活用する)
                ),
            },
            {
                blueFloorCloserGenerator()//青い床に差し掛かったら終了判定を出す
            }
        ),
        0
    );

    action17->setNext(action18);
    action18->setNext(action19);
    action19->setNext(action20);
    action20->setNext(action21);
    action21->setNext(action22);
    action22->setNext(action23);
    action23->setNext(action24);
    action24->setNext(action25);

    /**
     * ここからスマートキャリー
     */

    ActionNode* action26 = new ActionNode(
       "action26: 左90度を向く",
       &device,
       pivotTurnActionFactory(
           150.0f,
           !is_right,
           10,
           {
               timedCloserGenerator(100)
           }
       ),
       0
    );

    ActionNode* action27 = new ActionNode(
       "action27: 直進する",
       &device,
       goStraightActionFactory(
           800.0f,
           10,
           {},
           {
               runDistanceCloserGenerator(1000)
           }
       ),
       0
    );
    
    ActionNode* action28 = new ActionNode(
       "action28: 停止する",
       &device,
       stopActionFactory(),
       0
    );

    ActionNode* action29 = new ActionNode(
       "action29: 直進する、ターゲットサークルを視界の中に捉えるまで",
       &device,
       goStraightActionFactory(
           250.0f,
           50,
           {},
           {
               targetInImageCloserGenerator(TARGET_CIRCLE_IN_DISPLAY)
           }
       ),
       0
    );
    
    ActionNode* action30 = new ActionNode(
       "action30: 停止する",
       &device,
       stopActionFactory(),
       0
    );

    ActionNode* action31 = new ActionNode(
       "action31: その場で細かく角度を変えて、正面にターゲットサークルを見据える",
       &device,
       fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
       0
    );
    
    ActionNode* action32 = new ActionNode(
        "action32: ゆっくり前進して画面の中央よりも下にターゲットサークルが来るようにする",
        &device,
        goStraightActionFactory(
            250.0f,
            50,
            {},
            {
                obstacleNearCloserGenerator(TARGET_CIRCLE_XY)
            }
        ),
        0
    );
        
    ActionNode* action33 = new ActionNode(
       "action33: その場で細かく角度を変えて、正面にターゲットサークルを見据える",
       &device,
       fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
       0
    );

     ActionNode* action34 = new ActionNode(
        "action34: 直進する",
        &device,
        goStraightActionFactory(
            800.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(650)//本当は760mmだが、絶対にもっと近いところにあるので、少し短めの650mmで終了判定を出す
            }
        ),
        0
     );

     ActionNode* action35 = new ActionNode(
        "action35: 後退する",
        &device,
        goStraightActionFactory(
            -800.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(150)
            }
        ),
        0
     );

     ActionNode* action36 = new ActionNode(
        "action36: 少しだけ右に回転して今置いたペットボトルを視界から外す",
        &device,
        pivotTurnActionFactory(
            90.0f,
            is_right,
            10,
            {
                timedCloserGenerator(50)
            }
        ),
        0
     );

     ActionNode* action37 = new ActionNode(
        "action37: 停止する",
        &device,
        stopActionFactory(),
        0
     );

     ActionNode* action38 = new ActionNode(
        "action38: 右に回転しながら青いペットボトルを探す",
        &device,
        spinTurnActionFactory(
            45.0f,
            is_right,
            10,
            {
                targetInImageCloserGenerator(BLUE_BOTTLE_IN_FRONT)
            }
        ),
        0
     );
     
    ActionNode* action39 = new ActionNode(
        "action39: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    ActionNode* action40 = new ActionNode(
        "action40: 細かく角度を調節して、正面に青いペットボトルを見据える",
        &device,
        fineChangeDirectionActionFactory(BLUE_BOTTLE_XY),
        0
    );

    ActionNode* action41 = new ActionNode(
        "action41: ゆっくり前進して画面の中央よりも下に青いペットボトルが来るようにする",
        &device,
        goStraightActionFactory(
            250.0f,
            50,
            {},
            {
                obstacleNearCloserGenerator(BLUE_BOTTLE_XY)
            }
        ),
        0
     );
            
     ActionNode* action42 = new ActionNode(
        "action42: 細かく角度を調節して、正面に青いペットボトルを見据える",
        &device,
        fineChangeDirectionActionFactory(BLUE_BOTTLE_XY),
        0
     );

     ActionNode* action43 = new ActionNode(
        "action43: 直進してペットボトルを掴む",
        &device,
        goStraightActionFactory(
            800.0f,
            10,
            {},
            {
                obstacleCloserGenerator(80)
            }
        ),
        0
     );

     ActionNode* action44 = new ActionNode(
        "action44: 右90度を向く",
        &device,
        pivotTurnActionFactory(
            150.0f,
            is_right,
            10,
            {
                timedCloserGenerator(100)
            }
        ),
        0
     );
    
    ActionNode* action45 = new ActionNode(
        "action45: その場で細かく角度を変えて、正面にターゲットサークルを見据える",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action46 = new ActionNode(
        "action46: ゆっくり前進して画面の中央よりも下にターゲットサークルが来るようにする",
        &device,
        goStraightActionFactory(
            250.0f,
            50,
            {},
            {
                obstacleNearCloserGenerator(TARGET_CIRCLE_XY)
            }
        ),
        0
    );

    ActionNode* action47 = new ActionNode(
        "action47: その場で細かく角度を変えて、正面にターゲットサークルを見据える",
        &device,
        fineChangeDirectionActionFactory(TARGET_CIRCLE_XY),
        0
    );

    ActionNode* action48 = new ActionNode(
        "action48: 直進する",
        &device,
        goStraightActionFactory(
            800.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(650)//本当は760mmだが、絶対にもっと近いところにあるので、少し短めの650mmで終了判定を出す
            }
        ),
        0
     );

    ActionNode* action49 = new ActionNode(
        "action49: 後退する",
        &device,
        goStraightActionFactory(
            -800.0f,
            10,
            {},
            {
                runDistanceCloserGenerator(150)
            }
        ),
        0
     );

     ActionNode* action50 = new ActionNode(
        "action50: 右45度を向く",
        &device,
        pivotTurnActionFactory(
            90.0f,
            is_right,
            10,
            {
                timedCloserGenerator(50)
            }
        ),
        0
     );

     ActionNode* action51 = new ActionNode(
        "action51: 直進する",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {},
            {
                blackFloorCloserGenerator()
            }
        ),
        0
     );

    ActionNode* action52 = new ActionNode(
        "action52: その場で左に回転して正面に直線を検知する",
        &device,
        fineChangeDirectionLineActionFactory(!is_right),
        0
    );

    ActionNode* action53 = new ActionNode(
        "action53: 直線に沿って走行する、青い床に差し掛かるまで",
        &device,
        goStraightActionFactory(
            250.0f,
            10,
            {
                laneTracingAssistGenerator(
                    !is_right,
                    100.0f,
                    0.0f,
                    100.0f,
                    calcBlackWhiteBorderError
                )
            },
            {
                blueFloorCloserGenerator()
            }
        ),
        0
    );

    ActionNode* action54 = new ActionNode(
        "action54: 停止する",
        &device,
        stopActionFactory(),
        0
    );

    action25->setNext(action26);
    action26->setNext(action27);
    action27->setNext(action28);
    action28->setNext(action29);
    action29->setNext(action30);
    action30->setNext(action31);
    action31->setNext(action32);
    action32->setNext(action33);
    action33->setNext(action34);
    action34->setNext(action35);
    action35->setNext(action36);
    action36->setNext(action37);
    action37->setNext(action38);
    action38->setNext(action39);
    action39->setNext(action40);
    action40->setNext(action41);
    action41->setNext(action42);
    action42->setNext(action43);
    action43->setNext(action44);
    action44->setNext(action45);
    action45->setNext(action46);
    action46->setNext(action47);
    action47->setNext(action48);
    action48->setNext(action49);
    action49->setNext(action50);
    action50->setNext(action51);
    action51->setNext(action52);
    action52->setNext(action53);
    action53->setNext(action54);

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
    
    // タスク終了
    ext_tsk();
}