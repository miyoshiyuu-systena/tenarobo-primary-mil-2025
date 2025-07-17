#ifndef _ACTION_CHAIN_H_
#define _ACTION_CHAIN_H_

#include "TwinWheelDrive.h"
#include "Motor.h"
#include "share/PerceptionDataAccess.h"
#include <string>
#include <functional>

class ActionNode
{
public:
    /**
     * コンストラクタ
     * @param twinWheelDrive ツインホイールドライブのポインタ
     * @param frontArm 前腕モーターのポインタ
     * @param percDataAccess 知覚データへのアクセス（センサデータ）
     * @param actionCall アクションの実行
     *          一度実行すればいいだけの場合は、whileループを使用しない
     *          繰り返し実行する場合は、whileループを使用する
     *          コールバックの引数はこのActionNodeの次のポインタ
     * @param actionName アクションの名前（ログ出力のため）
     * @param vacationTime アクション終了後のクーリングタイム(ms)
     */
    ActionNode(
        TwinWheelDrive* twinWheelDrive,
        Motor* frontArm,
        PerceptionDataAccess& percDataAccess,
        std::function<void(ActionNode*&)> actionCall,
        std::string actionName,
        int vacationTime = 500
    );

    /**
     * デストラクタ
     * アクションの完全終了を宣言する
     */
    ~ActionNode();

    /**
     * アクションの実行
     */
    void execute();

    /**
     * アクションが終了したか
     */
    bool isEnd();

    /**
     * 次のアクションの設定
     */
    void setNext(ActionNode* nextAction);

    /**
     * 次のアクションの取得
     */
    ActionNode* getNext();

    /**
     * 次以降のアクションの削除
     */
    void deleteNext();

private:
    /**
     * 周辺モジュールへのアクセス
     */
    TwinWheelDrive* mTwinWheelDrive;
    Motor* mFrontArm;

    /**
     * 知覚データへのアクセス
     */
    PerceptionDataAccess* mPercDataAccess;
    
    /**
     * アクションの名前
     */
    std::string mActionName;

    /**
     * アクションの実行
     */
    std::function<void(ActionNode*&)> mActionCall;


    /**
     * 次のアクション
     */
    ActionNode* mNextAction;
    
    /**
     * このアクションチェーンが終了している
     */
    bool mIsEnd;

    /**
     * アクション終了時のクーリングタイム
     * モーターが完全に停止する前に次の処理をすると、想定よりも進まないということがある
     */
    int mVacationTime;
};

#endif // _ACTION_CHAIN_H_