"""
IAnaS
Image Analysis Server
ここでは画像解析を実行する
特定の名前のついた共有メモリから最新の画像を読み込む
解析結果を特定の名前のついた共有メモリに書き込む
"""

import cv2
import numpy as np
import posix_ipc
import time
import mmap
import struct
from enum import Enum

# 画像の共有メモリの名前
# プロセス間で名前を統一する必要がある
SHM_IMAGE_NAME = "/robot_shm_image"
SEM_IMAGE_NAME = "/robot_sem_image"

SHM_ANALYSIS_COMMAND_NAME = "/robot_shm_analysis_command"
SEM_ANALYSIS_COMMAND_NAME = "/robot_sem_analysis_command"

SHM_ANALYSIS_RESULT_NAME = "/robot_shm_analysis_result"
SEM_ANALYSIS_RESULT_NAME = "/robot_sem_analysis_result"

class ANALYSIS_COMMAND(Enum):
    ## 画像処理をしない
    do_nothing = 0

    ## 前方に直線がある
    front_straight = 1

    ## ラインの右縁に乗っている
    on_right_edge = 2

    ## ラインの左縁に乗っている
    on_left_edge = 3

    ## 正面にゲートがある
    gate_in_front = 4

    ## 正面に赤いペットボトルがある。
    red_bottle_in_front = 5

    ## 正面に青いペットボトルがある。
    blue_bottle_in_front = 6

# 分析結果パケット
#  bit0: 前方に直線がある
#  bit1: ラインの右縁に乗っている
#  bit2: ラインの左縁に乗っている
#  bit3: 正面にゲートがある
#  bit4: 正面に赤いペットボトルがある
#  bit5: 正面に青いペットボトルがある
#  bit6: 予約
#  bit7: 予約
#
# ↑　このデータの配置を考慮して実装する
FRONT_STRAIGHT_MASK             = 1 << 0 # 前方に直線がある
ON_RIGHT_EDGE_MASK              = 1 << 1 # ラインの右縁に乗っている 
ON_LEFT_EDGE_MASK               = 1 << 2 # ラインの左縁に乗っている
GATE_IN_FRONT_MASK              = 1 << 3 # 正面にゲートがある
RED_BOTTLE_IN_FRONT_MASK        = 1 << 4 # 正面に赤いペットボトルがある
BLUE_BOTTLE_IN_FRONT_MASK       = 1 << 5 # 正面に青いペットボトルがある
RESERVED6_MASK                  = 1 << 6 # 予約
RESERVED7_MASK                  = 1 << 7 # 予約

# @note
#  Pythonには構文としての構造体やビットフィールドが存在しないため、静的にデータの配置を定義づけられない。
#  データのの配置を変えるときは、他のプログラムとの合意を維持していることをよく確認しなければならない。

# 分析結果パケットのフォーマット
# B: 1バイトのデータ
#### ↑　現在は6bitしか使っていないので1byteで事足りている
#### 今後機能拡張していく際はフォーマットを変更する
format_string = "<B"

def is_gate_in_front(image):
    # 画像をグレースケールに変換
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_gray.png", image)

    # ガウシアンぼかし
    # 周辺のピクセルを取り込んでごちゃ混ぜにする
    # これによってノイズを軽減する
    image = cv2.GaussianBlur(image, (5, 151), 0)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_blur.png", image)

    # 二値化
    # 明るいところを255、暗いところを0に変換する
    image = cv2.inRange(image, 0, 127)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_binary.png", image)

    is_left_post_start_found = False
    is_left_post_end_found = False
    is_right_post_start_found = False

    # 左ポストの終了位置
    # 右ポストの開始位置
    # を探す
    left_post_index = -1
    right_post_index = -1

    image_row = image[image.shape[0] // 3]
    for pixel_index, pixel in enumerate(image_row):
        # 画像の上から1/3のラインを抽出し、
        # 左側から1pixelずつ見ていく
        if is_left_post_start_found == False:
            # 左のポストの開始位置を探す
            if pixel == 255:
                # ピクセルが明るいならば、初めて左のポストが見つかったと考える
                is_left_post_start_found = True
        elif is_left_post_end_found == False:
            # 左のポストの終了位置を探す
            if pixel == 0:
                # ピクセルが暗いならば、左のポストが見つかったと考える
                is_left_post_end_found = True
                left_post_index = pixel_index
        elif is_right_post_start_found == False:
            # 右のポストの開始位置を探す
            if pixel == 255:
                # ピクセルが明るいならば、初めて右のポストが見つかったと考える
                is_right_post_start_found = True
                right_post_index = pixel_index
        else:
            # 全て見つかっているのでゲートは見つかっている
            break

    if left_post_index == -1 or right_post_index == -1:
        # ゲートが見つかっていない
        return False

    # ゲートの中心位置を計算
    center_index = (left_post_index + right_post_index) // 2

    # ゲートの中心位置が画像の中央付近にあるかどうかを返す
    return ((image.shape[1] * 2 // 5) <= center_index) and (center_index <= (image.shape[1] * 3 // 5))

### TODO 動作チェック
def is_front_straight(image):
    ## 画像をグレースケールに変換
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_gray.png", image)

    ## ガウシアンぼかし
    ## 周辺のピクセルを取り込んでごちゃ混ぜにする
    ## これによってノイズを軽減する
    image = cv2.GaussianBlur(image, (5, 5), 0)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_blur.png", image)

    ## エッジ検出
    ## エッジを検出する
    image = cv2.Canny(image, 100, 200)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_canny.png", image)

    ## ハフ変換
    ## 直線を検出する
    lines = cv2.HoughLinesP(image, 1, np.pi / 180, 50, 10, 10)
    
    # 直線が検出されなかった場合の処理を追加
    if lines is None:
        return False  # 直線が検出されなかった場合はFalseを返す
    
    # 直線が検出された場合の処理
    lines_squeezed = lines.squeeze()
    if lines_squeezed.ndim == 1:  # 1つの直線のみ検出された場合
        lines_squeezed = lines_squeezed.reshape(1, -1)

    is_front_straight = False
    _image = np.zeros((image.shape[0], image.shape[1]), dtype=np.uint8)
    for x1, y1, x2, y2 in lines_squeezed:
        ## x1 始点のx座標
        ## y1 始点のy座標
        ## x2 終点のx座標
        ## y2 終点のy座標
        if (
            ((image.shape[1] * 1 // 5) < x1 < (image.shape[1] * 4 // 5) and (image.shape[0] * 4 // 5) < y1) or
            ((image.shape[1] * 1 // 5) < x2 < (image.shape[1] * 4 // 5) and (image.shape[0] * 4 // 5) < y2)
        ):
            ## 線分のx座標が画像の左端1/5から右端1/5の間（比較的中央付近）であり
            ## かつ、その始点のy座標は画像の上端から4/5の位置より下にある

            ## この条件は何を指しているかというと
            ## ロボットの足元から伸びている、ある程度長い直線があるかどうかを判定する
            cv2.line(_image, (x1, y1), (x2, y2), (255, 0, 0), 4)
            is_front_straight = True
    
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_hough.png", _image)
    return is_front_straight



def main():
    # 画像の共有メモリ・セマフォの取得
    shm_image = posix_ipc.SharedMemory(SHM_IMAGE_NAME)
    shm_image_map = mmap.mmap(shm_image.fd, shm_image.size)
    sem_image = posix_ipc.Semaphore(SEM_IMAGE_NAME)

    # 分析コマンドの共有メモリ・セマフォの作成
    shm_analysis_command = posix_ipc.SharedMemory(SHM_ANALYSIS_COMMAND_NAME, posix_ipc.O_CREAT, size=4)
    shm_analysis_command_map = mmap.mmap(shm_analysis_command.fd, shm_analysis_command.size)
    sem_analysis_command = posix_ipc.Semaphore(SEM_ANALYSIS_COMMAND_NAME, posix_ipc.O_CREAT, initial_value=1)
    sem_analysis_command.acquire()
    shm_analysis_command_map[:] = struct.pack('<i', ANALYSIS_COMMAND.do_nothing.value)
    sem_analysis_command.release()

    # 分析結果の共有メモリ・セマフォの作成
    shm_analysis_result = posix_ipc.SharedMemory(SHM_ANALYSIS_RESULT_NAME, posix_ipc.O_CREAT, size=struct.calcsize(format_string))
    shm_analysis_result_map = mmap.mmap(shm_analysis_result.fd, shm_analysis_result.size)
    sem_analysis_result = posix_ipc.Semaphore(SEM_ANALYSIS_RESULT_NAME, posix_ipc.O_CREAT, initial_value=1)

    print("IAnaS: バックグラウンドで画像分析タスクを開始しました。")
    print("IAnaS: 終了するにはCtrl+Cを押してください。")

    try:
        count = 0
        while True:
            # 分析コマンドを読み込む
            sem_analysis_command.acquire()
            # 共有メモリから4バイトを読み取り、int型に変換
            # '<i'はリトルエンディアンのsigned intを意味
            command_bytes = shm_analysis_command_map[:4]
            command = struct.unpack('<i', command_bytes)[0]
            sem_analysis_command.release()

            if (count % 100 == 0):
                print("IAnaS: 分析", command, "を実行しています")

            # 分析コマンドがない場合はスキップ
            if (command == ANALYSIS_COMMAND.do_nothing.value):
                # 他のプロセスが読み取りを終えるまで待つ（セマフォをロック）
                sem_analysis_result.acquire()
                # 共有メモリを0でクリア
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, 0)
                sem_analysis_result.release()
                # 書き込みが完了したらセマフォをアンロック
                time.sleep(1)
                continue

            # セマフォをロック
            sem_image.acquire()

            # 共有メモリから画像を読み込む
            width = 320
            height = 240
            image = np.frombuffer(shm_image_map, dtype=np.uint8).reshape(height, width, 3)

            # セマフォを解放
            sem_image.release()

            if (command == ANALYSIS_COMMAND.front_straight.value):
                result = 0
                front_straight_detected = is_front_straight(image)
                if front_straight_detected:
                    result |= FRONT_STRAIGHT_MASK
                else:
                    result &= ~FRONT_STRAIGHT_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.on_right_edge.value):
                result = 0
                result |= ON_RIGHT_EDGE_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.on_left_edge.value):
                result = 0
                result |= ON_LEFT_EDGE_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()
                
            elif (command == ANALYSIS_COMMAND.gate_in_front.value):
                result = 0
                gate_detected = is_gate_in_front(image)
                if gate_detected:
                    result |= GATE_IN_FRONT_MASK
                else:
                    result &= ~GATE_IN_FRONT_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.red_bottle_in_front.value):
                ### mada
                ### mada
                ### mada
                ### mada
                ### mada
                result = 0
                result |= RED_BOTTLE_IN_FRONT_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.blue_bottle_in_front.value):
                ### mada
                ### mada
                ### mada
                ### mada
                ### mada
                result = 0
                result |= BLUE_BOTTLE_IN_FRONT_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result)
                sem_analysis_result.release()

            else:
                print("IAnaS: 合意のないコマンドです: ", command)
                print("IAnaS: 開発者はインターフェースを見直して下さい")

            count += 1
            time.sleep(0.033) # 30fpsで画像を分析

    finally:
        shm_image_map.close()
        shm_image.close_fd()
        shm_image.unlink() # 終了時に共有メモリを削除
        sem_image.close()
        sem_image.unlink() # 終了時にセマフォを削除


if __name__ == "__main__":
    main()
