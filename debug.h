#ifndef _DEBUG_H_
#define _DEBUG_H_

/**
 * デバッグ動作の有効/無効
 * 
 * @note
 *  コンパイルする前に、各変数の値を調整して、記録を残したりできる
 */

/**
 * 知覚タスクのログ出力の有効/無効
 * デバッグ用
 */
const bool isPerceptionLoggingEnable = false;

/**
 * 知覚タスクをログ出力するとき、マスクを無視して全てのデータを取得するか
 * デバッグ用
 */
const bool isPerceptionLoggingIgnoreMask = isPerceptionLoggingEnable && false;

/**
 * Webカメラの画像を保存するか
 * デバッグ用
 */
const bool isSaveCameraImage = false;

#endif // _DEBUG_H_