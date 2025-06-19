#ifndef _ACTION_NODE_H_
#define _ACTION_NODE_H_

class ActionNode;

#include "perception/Perception.h"
#include "drive/TwinWheelDrive.h"
#include "drive/FrontArmDrive.h"
#include <string>
#include <functional>

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
     * @param perc_mask 知覚データのマスク
     * @param actionCall アクションのコールバック関数
     * @param vacationTime アクション終了後のクーリングタイム(ms)
     */
    ActionNode(
        std::string actionName,
        TwinWheelDrive* twinWheelDrive,
        FrontArmDrive* frontArmDrive,
        Perception* perc,
        uint8_t perc_mask,
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
     * 次のアクションの取得
     * @return 次のアクションのポインタ
     */
    ActionNode* getNext();

    /**
     * 次のアクションの削除
     */
    void deleteNext();

private:
    std::string mActionName;
    ActionNode* mNextAction;
    TwinWheelDrive* mTwinWheelDrive;
    FrontArmDrive* mFrontArmDrive;
    Perception* mPerc;
    uint8_t mPercMask;
    ActionCall mActionCall;
    int mVacationTime;
};

#endif // _ACTION_NODE_H_