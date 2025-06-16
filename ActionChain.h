#ifndef _ACTION_CHAIN_H_
#define _ACTION_CHAIN_H_

#include "TwinWheelDrive.h"
#include "Motor.h"
#include "PercDataAccess.h"
#include <string>

class ActionChain
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
     * @param actionName アクションの名前（ログ出力のため）
     */
    ActionChain(
        TwinWheelDrive* twinWheelDrive,
        Motor* frontArm,
        PercDataAccess& percDataAccess,
        void (*actionCall)(void),
        std::string actionName
    );

    /**
     * デストラクタ
     * アクションの完全終了を宣言する
     */
    ~ActionChain();

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
    void setNext(ActionChain* nextAction);

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
    PercDataAccess* mPercDataAccess;
    
    /**
     * アクションの名前
     */
    std::string mActionName;

    /**
     * アクションの実行
     */
    void (*mActionCall)(void);


    /**
     * 次のアクション
     */
    ActionChain* mNextAction;
    
    /**
     * このアクションチェーンが終了している
     */
    bool mIsEnd;
};

#endif // _ACTION_CHAIN_H_