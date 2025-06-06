#include    "app.h"
#include    <stdio.h>
#include    "syssvc/syslog.h"
#include    "PerceptionTask.h"
#include    "DrivingTask.h"

/**
 * メイン処理
 * @param   exinf     拡張情報
 */
void    main_task(intptr_t exinf)   {
    /**
     * センサーデータ取得インスタンスの生成
     */
    //  TODO miyoshi: センサーの初期化

    /**
     * シナリオチェーンの構成
     */
    //  TODO miyoshi: シナリオチェーンの構成

    /**
     * 駆動系インスタンスの生成
     */
    //  TODO miyoshi: 駆動系インスタンスの生成

    syslog(LOG_NOTICE, "INIT FINISH - Its start!!!");

    sta_cyc(PERC_CYC);
    sta_cyc(DRIV_CYC);

    while(
        (1) &&  /* 停止ボタン（圧力センサ）が押されていない */  //TODO: 仮に無限ループとする  
        (1)     /* 全てのタスクが終了した or 異常終了した */    //TODO: 仮に無限ループとする
    ) {
        /**
         * 通信部 -> 制御部データコピー
         */
        //  TODO    miyoshi: 通信部 -> 制御部データコピー

        /**
         * 意思決定
         */
        //  TODO    miyoshi: 意思決定

        /**
         * 制御部 -> 通信部データコピー
         */
        //  TODO    miyoshi: 制御部 -> 通信部データコピー

        /**
         * 待機
         */
        // syslog(LOG_NOTICE, "Main_Task is running");
    }
    
    stp_cyc(PERC_CYC);
    stp_cyc(DRIV_CYC);
    
    //  タスク終了
    ext_tsk();
}