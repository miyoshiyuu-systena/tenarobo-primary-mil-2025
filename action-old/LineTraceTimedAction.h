#include    "action/ActionNode.h"
#include    <functional>

/**
 * ライントレースアクションを生成するファクトリー関数
 * 特定の時間が経過したら停止する
 * @param speed 左右の輪の速度[mm/s]([0]は左輪、[1]は右輪)
 * @param isRightSide 右寄りか左寄りか
 * @param duration 判定周期[ms]
 * @param Kp 比例係数
 * @param Ki 積分係数
 * @param Kd 微分係数
 * @param stop_time 停止時間[ms]
 * @param calc_error 誤差の計算
 */
std::function<void(ActionNode*&)> line_trace_timed_action(
    float* speed,
    bool is_right_side,
    int duration,
    /**
     * 比例定数
     * @note
     *      比例定数を大きくすると
     *      ラインから外れかけたときに鋭く補正がかかる
     *      ただし、大きく方向が変わるなどの問題もあり得る
     */
    float Kp,
    /**
     * 積分定数
     * @note
     *      積分定数を使用することで定常誤差を減らし、より精密なライントレースが可能になる
     *      値を大きくすると過去のエラーの累積効果が強くなるが、
     *      過度に大きくすると振動やオーバーシュートを引き起こす可能性がある
     */
    float Ki,
    /**
     * 微分定数
     * @note
     *      微分定数を使用することでエラーの変化率に基づく予測的制御が可能になる
     *      オーバーシュートを抑制し、より滑らかな応答を実現する
     *      ただし、値を大きくしすぎるとノイズに敏感になり不安定になる可能性がある
     */
    float Kd,
    int stop_time,
    std::function<float(int h, int s, int v)> calc_error
);