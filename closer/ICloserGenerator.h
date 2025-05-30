#ifndef I_CLOSER_GENERATOR_H
#define I_CLOSER_GENERATOR_H

#include "ICloser.h"
#include <functional>
#include "Device.h"

/**
 * 終了判定クラスのファクトリー関数
 * @note
 * 使用する寸前までインスタンス化を抑えることで記憶領域を節約する目的で実装されたジェネレータパターン。
 * 
 * 主な目的：
 * - メモリ節約：実際に使用されるまでオブジェクトを生成しない遅延インスタンス化
 * - 自動メモリ管理：使用後の確実な削除によりメモリリークを防止
 * - 柔軟性：異なるパラメータでの複数のジェネレータを事前定義可能
 * - 型安全性：std::functionによる型安全なファクトリー関数
 * 
 * 組み込みシステムにおけるリアルタイム制御では、このような細やかなメモリ管理が重要。
 */
typedef std::function<ICloser*(Device*&)> ICloserGenerator;

#endif 