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

    ## 画面内にターゲットサークルがある
    target_circle_in_display = 2

    ## ラインの左縁に乗っている
    on_left_edge = 3

    ## 正面にゲートがある
    gate_in_front = 4

    ## 正面に赤いペットボトルがある。
    red_bottle_in_front = 5

    ## 正面に青いペットボトルがある。
    blue_bottle_in_front = 6

    ## ターゲットサークルの座標
    target_circle_xy = 7

    ## 青いペットボトルの座標
    blue_bottle_xy = 8

# 分析結果パケット
#  bit0: 前方に直線がある
#  bit1: 画面内にターゲットサークルがある
#  bit2: ラインの左縁に乗っている
#  bit3: 正面にゲートがある
#  bit4: 正面に赤いペットボトルがある
#  bit5: 正面に青いペットボトルがある
#  bit6: 予約
#  bit7: 予約
#  uint16: ターゲットサークルのX座標 （画像のサイズは320x240で1byte(uint8)では表現できない）
#  uint16: ターゲットサークルのY座標 
#  uint16: 青いペットボトルのX座標 
#  uint16: 青いペットボトルのY座標 
#
# ↑　このデータの配置を考慮して実装する
FRONT_STRAIGHT_MASK             = 1 << 0 # 前方に直線がある
TARGET_CIRCLE_IN_DISPLAY_MASK   = 1 << 1 # 画面内にターゲットサークルがある
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
# H: 2バイトのデータ
# https://docs.python.org/ja/3.13/library/struct.html

#### 画像分析の種類を増やすたびに、C++側と合わせてここを変更すること
# C++構造体のメモリレイアウトに合わせる:
# Byte 0: ビットフィールド(B), Byte 1: パディング(x), Bytes 2-9: 4つのuint16_t(HHHH)
format_string = "<BxHHHH"

def is_gate_in_front(image):
    # 画像をグレースケールに変換
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_gray.png", image)

    # ガウシアンぼかし
    # 周辺のピクセルを取り込んでごちゃ混ぜにする
    # これによってノイズを軽減する
    image = cv2.GaussianBlur(image, (5, 501), 0)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_blur.png", image)

    # 二値化
    # 明るいところを255、暗いところを0に変換する
    image = cv2.inRange(image, 120, 255)
    # image = cv2.inRange(image, 100, 255)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/0_binary.png", image)

    is_left_post_start_found = False
    is_left_post_end_found = False
    is_right_post_start_found = False

     # 左ポストの終了位置
    # 右ポストの開始位置
    # を探す
    left_post_index = -1
    right_post_index = -1

    for i in range(image.shape[1]):
        bright = image[image.shape[0] // 6, i]
        # bright = image[image.shape[0] // 3, i]
        if is_left_post_start_found == False:
            if bright > 127:
                pass
            else:
                is_left_post_start_found = True
                left_post_index = i
        elif is_left_post_end_found == False:
            if bright < 127:
                pass
            else:
                is_left_post_end_found = True
        elif is_right_post_start_found == False:
            if bright > 127:
                pass
            else:
                is_right_post_start_found = True
                right_post_index = i
        else:
            ## Nothing
            break

    if left_post_index == -1 or right_post_index == -1:
        # ゲートが見つかっていない
        print("no center")
        return False

    # ゲートの中心位置を計算
    center_index = (left_post_index + right_post_index) // 2
    print(f"center_index: {center_index}, is_gate_in_front: {((image.shape[1] * 2 // 5) <= center_index) and (center_index <= (image.shape[1] * 3 // 5))}")

    # ゲートの中心位置が画像の中央付近にあるかどうかを返す
    return ((image.shape[1] * 2 // 5) <= center_index) and (center_index <= (image.shape[1] * 3 // 5))

def is_front_straight(row_image):
    is_front_straight = False

    ## ガウシアンぼかし
    ## 周辺のピクセルを取り込んでごちゃ混ぜにする
    ## これによってノイズを軽減する
    image = cv2.GaussianBlur(row_image, (5, 5), 0)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_blur.png", image)
    
    ## 画像をグレースケールに変換
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_gray.png", image)

    ## 画像を2値化
    image = cv2.inRange(image, (0, 0, 0), (360, 255, 127))
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_binary.png", image)

    ## エッジ検出
    ## エッジを検出する
    image = cv2.Canny(image, 50, 150)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_canny.png", image)

    ## ハフ変換
    ## 直線を検出する
    lines = cv2.HoughLinesP(image, 1, np.pi / 180, 20, 25, 10)
    
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            ## ロボットの足元から伸びている直線だけを抽出
            if (
                ((image.shape[1] * 2 / 5) < x1) and (x1 < (image.shape[1] * 3 / 5)) and ((image.shape[0] * 5 / 7) < y1)
                or ((image.shape[1] * 2 / 5) < x2) and (x2 < (image.shape[1] * 3 / 5)) and ((image.shape[0] * 5 / 7) < y2)
            ):
                ## 正面に向かってまっすぐ伸びている直線だけを抽出
                if np.abs(np.tan(np.arctan2(x2 - x1, y2 - y1))) < 0.1:
                    is_front_straight = True
                    cv2.line(row_image, (x1, y1), (x2, y2), (127, 255, 0), 2)

    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_hough.png", row_image)

    return is_front_straight

def get_target_circle_center(image):
    image3 = cv2.GaussianBlur(image, (5, 5), 0)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/2_blur.png", image3)

    # image4 = cv2.inRange(image3, (50, 50, 95), (255, 255, 125))
    image4 = cv2.inRange(image3, (50, 50, 40), (255, 255, 100))
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/2_binary.png", image4)

    contours, _ = cv2.findContours(image4, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    image5 = cv2.cvtColor(image3, cv2.COLOR_RGB2GRAY)

    target_x = 0
    target_y = 0

    for contour in contours:
        ## 輪郭の面積
        area = cv2.contourArea(contour)

        if area > 2:
            ## 小さすぎるものは除外（面積1はモメントの計算で0になり、0除算のエラーを起こすことがあるので、諦める）
            ## 重心
            m = cv2.moments(contour)

            if m['m00'] != 0:
                ## ちゃんとモメントが得られたものはターゲットサークルの判定に進む
                x, y= m['m10']/m['m00'] , m['m01']/m['m00']

                # 座標を四捨五入する
                x = round(x)
                y = round(y)

                if y < (image.shape[0] // 6):
                    continue

                ## 重心の周辺に輪っかがあることを調べる
                ## この方法は縁の端っこが画面外の時は検出できない
                ## 円に一番近い内側の1つの円だけに注目する
                ## 2, 3重円を検出しようとすると、画面外の問題で検出ハードルが上がってしまう（誤検出は減るだろうね）
                scan = 1
                left_peak = 0
                right_peak = 0
                prev_left_bright = image5[y, x]
                prev_prev_left_bright = image5[y, x]
                prev_right_bright = image5[y, x]
                prev_prev_right_bright = image5[y, x]
                while (True):
                    left = x - scan
                    right = x + scan
                    if (left < 0) or (right >= image5.shape[1]):
                        # 画面外は諦める
                        break

                    if left_peak == 0:
                        # まだ左のピークを探索中
                        # uint8のアンダーフローを防ぐためint型にキャスト
                        diff_left = (int(image5[y, left]) - int(prev_left_bright)) + (int(prev_left_bright) - int(prev_prev_left_bright))
                        if diff_left < -3:
                            left_peak = left

                    if right_peak == 0:
                        # まだ右のピークを探索中
                        # uint8のアンダーフローを防ぐためint型にキャスト
                        diff_right = (int(image5[y, right]) - int(prev_right_bright)) + (int(prev_right_bright) - int(prev_prev_right_bright))
                        if diff_right < -3:
                            right_peak = right

                    if (left_peak != 0) and (right_peak != 0):
                        # 重心の左右の円の縁を検出
                        if (((right_peak - x) * 0.9) < (x - left_peak)) and ((x - left_peak) < ((right_peak - x) * 1.1)):
                            # 重心を中心として正円がまとわりついている
                            # これはターゲットサークルに違いない
                            target_x = x
                            target_y = y
                            cv2.line(image, (x - 5, y - 5), (x + 5, y + 5), (255, 255, 127), 2)
                            cv2.line(image, (x + 5, y - 5), (x - 5, y + 5), (255, 255, 127), 2)
                            cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/2_target.png", image)
                        break

                    # 玉ねぎの皮みたいに
                    # 内側から1枚ずつ円を探してみる
                    scan = scan + 1
                    # 前回の明るさを記録する
                    prev_prev_left_bright = prev_left_bright
                    prev_left_bright = image5[y, left]
                    prev_prev_right_bright = prev_right_bright
                    prev_right_bright = image5[y, right]

    return target_x, target_y

def is_target_circle_in_display(image):
    target_x, target_y = get_target_circle_center(image)
    return target_x != 0 and target_y != 0

def get_blue_bottle_center(image):
    image2 = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    image3 = cv2.GaussianBlur(image2, (5, 5), 0)
    image4 = cv2.inRange(image3, (95, 50, 50), (125, 255, 255))

    contours, _ = cv2.findContours(image4, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    bottle_index = 0
    bottle_area = 0
    for i, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if bottle_area < area:
            bottle_index = i
            bottle_area = area
            # 最も大きいものをペットボトルとする
            ## 確かに大会の会場にある青いものの中でもペットボトルのラベルは大きい方だが
            ## 本当は良くなくて
            ## ペットボトルには青いラベルがついているだけでなく
            ## ラベルは正面から見れば四角形に見えるし
            ## ペットボトル特有の形状があるし
            ## 白いキャップも付いている
            ## いろんな分析を統合して、ペットボトルの偽陽性を減らしたい

    m = cv2.moments(contours[bottle_index])
    # モーメントが0の場合、ペットボトルではないと判断する
    # モーメントが0でない場合、ペットボトルと判断し、重心を計算する
    if bottle_area > 0 and m['m00'] != 0:
        x = round(m['m10'] / m['m00'])
        y = round(m['m01'] / m['m00'])
        return x, y

    return 0, 0

def is_blue_bottle_in_front(image):
    x, y = get_blue_bottle_center(image)
    return x != 0 and y != 0


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
                shm_analysis_result_map[:] = struct.pack(format_string, 0, 0, 0, 0, 0)
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
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.target_circle_in_display.value):
                result = 0
                target_circle_detected = is_target_circle_in_display(image)
                if target_circle_detected:
                    result |= TARGET_CIRCLE_IN_DISPLAY_MASK
                else:
                    result &= ~TARGET_CIRCLE_IN_DISPLAY_MASK
                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.on_left_edge.value):
                result = 0
                result |= ON_LEFT_EDGE_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
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
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
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
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.blue_bottle_in_front.value):
                result = 0
                blue_bottle_detected = is_blue_bottle_in_front(image)
                if blue_bottle_detected:
                    result |= BLUE_BOTTLE_IN_FRONT_MASK
                else:
                    result &= ~BLUE_BOTTLE_IN_FRONT_MASK

                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, result, 0, 0, 0, 0)
                sem_analysis_result.release()
            
            elif (command == ANALYSIS_COMMAND.target_circle_xy.value):
                result_x, result_y = get_target_circle_center(image)
                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                # C++構造体: ビットフィールド(0), target_circle_x(result_x), target_circle_y(result_y), blue_bottle_x(0), blue_bottle_y(0)
                shm_analysis_result_map[:] = struct.pack(format_string, 0, result_x, result_y, 0, 0)
                sem_analysis_result.release()

            elif (command == ANALYSIS_COMMAND.blue_bottle_xy.value):
                result_x, result_y = get_blue_bottle_center(image)
                sem_analysis_result.acquire()
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, 0, 0, 0, result_x, result_y)
                sem_analysis_result.release()

            else:
                print("IAnaS: 合意のないコマンドです: ", command)
                print("IAnaS: 開発者はインターフェースを見直して下さい")

            count += 1
            time.sleep(0.1) # 0.1sごとに画像を分析

    finally:
        shm_image_map.close()
        shm_image.close_fd()
        shm_image.unlink() # 終了時に共有メモリを削除
        sem_image.close()
        sem_image.unlink() # 終了時にセマフォを削除


if __name__ == "__main__":
    main()
