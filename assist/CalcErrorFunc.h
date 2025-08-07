#ifndef CALC_ERROR_FUNC_H
#define CALC_ERROR_FUNC_H

#include <functional>

/**
 * 誤差を計算する関数の型
 */
typedef std::function<float(int, int, int)> CalcErrorFunc;

#endif // CALC_ERROR_FUNC_H