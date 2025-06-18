#ifndef _ACTION_NODE_H_
#define _ACTION_NODE_H_

#include "Perception.h"
#include "TwinWheelDrive.h"
#include "FrontArmDrive.h"

/**
 * アクションのコールバック関数の型定義
 * @param currAction 現在のアクションのポインタ（必ず自分自身のポインタとなる）
 * @note
 *  自分自身を渡すのは、自分のタスクを一旦中断して、後から再実施する、みたいな動きを実現するため
 *  例えば、ライントレースでラインから大きく外れたとき、一旦ラインを探すアクションを実行してから、もう一度ライントレースを実行する
 * @param nextAction 次のアクションのポインタ(自分自身に登録されているポインタを渡す)
 * @param twinWheelDrive ツインホイールドライブのポインタ
 * @param frontArmDrive フロントアームドライブのポインタ
 * @param percData 知覚データのポインタ
 */
typedef std::function<void(
    ActionNode*&,
    ActionNode*&,
    TwinWheelDrive*&,
    FrontArmDrive*&,
    Perception*&
)> ActionCall;

/**
 * アクションノード
 * nextに次のノードのポインタを与えることで、アクションの連続を表現する
 * アクションの連鎖をアクションチェーンと呼称する
 */
class ActionNode
{
public:
    /**
     * コンストラクタ
     * このインスタンスが生成されたときに実行する
     * @param actionName アクションの名前
     * @param twinWheelDrive 2輪インスタンスへのポインタ
     * @param frontArmDrive フロントアームへのポインタ
     * @param percData 知覚データのポインタ
     * @param actionCall アクションのコールバック関数
     * @param vacationTime アクション終了後のクーリングタイム(ms)
     */
    ActionNode(
        std::string actionName,
        TwinWheelDrive* twinWheelDrive,
        FrontArmDrive* frontArmDrive,
        Perception* perc,
        ActionCall actionCall,
        int vacationTime
    );

    /**
     * デストラクタ
     * このインスタンスが消滅するときに実行する
     */
    ~ActionNode();

    /**
     * アクションの実行
     */
    void execute();

    /**
     * 次のアクションの設定
     * @param nextAction 次のアクションのポインタ
     */
    void setNext(ActionNode* nextAction);

    /**
     * 次のアクションの削除
     */
    void deleteNext();
};

#endif // _ACTION_NODE_H_