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
import matplotlib.pyplot as plt
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
    image = cv2.inRange(image, 0, 127)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_binary.png", image)

    ## エッジ検出
    ## エッジを検出する
    image = cv2.Canny(image, 50, 150)
    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_canny.png", image)

    ## ハフ変換
    ## 直線を検出する
    lines = cv2.HoughLinesP(image, 1, np.pi / 180, 20, 40, 10)
    
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            ## ロボットの足元から伸びている直線だけを抽出
            if (
                ((image.shape[1] * 2 / 7) < x1) and (x1 < (image.shape[1] * 5 / 7)) and ((image.shape[0] * 4 / 7) < y1)
                or ((image.shape[1] * 2 / 7) < x2) and (x2 < (image.shape[1] * 5 / 7)) and ((image.shape[0] * 4 / 7) < y2)
            ):
                ## 正面に向かってまっすぐ伸びている直線だけを抽出
                if np.abs(np.tan(np.arctan2(x2 - x1, y2 - y1))) < 0.20:
                    is_front_straight = True
                    cv2.line(row_image, (x1, y1), (x2, y2), (127, 255, 0), 2)

    cv2.imwrite("/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/1_hough.png", row_image)

    return is_front_straight

def has_blue_nearby(
    image,
    ellipse,
    margin=8.0,
    exclude_enlarge_factor=1.3,
    lower_hsv=np.array([100, 100, 150]),
    upper_hsv=np.array([140, 255, 255])):
    """ 付近の青色有無

    Args:
        image (image): 画像
        ellipse (Ellipse): 楕円
        margin (float, optional): 青色探索範囲. Defaults to 8.0.
        exclude_enlarge_factor (float, optional): 探索除外範囲倍率. Defaults to 1.3.
        lower_hsv (NDArray, optional): 青色下限HSV値. Defaults to np.array([100, 100, 150]).
        upper_hsv (NDArray, optional): 青色上限HSV値. Defaults to np.array([140, 255, 255]).

    Returns:
        bool 付近の青色有無
    """
    
    # 探索範囲を算出し切り出し
    center = ellipse[0]
    size = ellipse[1]
    x, y = int(center[0]), int(center[1])
    h, w = int(size[0] * margin), int(size[1] * margin)
    x1 = max(x - w // 2, 0)
    y1 = max(y - h // 2, 0)
    x2 = min(x + w // 2, image.shape[1])
    y2 = min(y + h // 2, image.shape[0])
    roi = image[y1:y2, x1:x2].copy()

    # 探索除外範囲を塗りつぶしたマスクを作成
    ellipse_mask = np.zeros(roi.shape[:2], dtype=np.uint8)
    roi_center = (x - x1, y - y1)
    excluded_ellipse_size_w = int(size[0] / 2 * exclude_enlarge_factor)
    excluded_ellipse_size_h = int(size[1] / 2 * exclude_enlarge_factor)
    cv2.ellipse(ellipse_mask, roi_center, (excluded_ellipse_size_w, excluded_ellipse_size_h), ellipse[2], 0, 360, 255, -1)
    ellipse_mask = cv2.bitwise_not(ellipse_mask)
    masked_roi = cv2.bitwise_and(roi, roi, mask=ellipse_mask)

    # BGRからHSV色空間に変換
    hsv_image = cv2.cvtColor(masked_roi, cv2.COLOR_BGR2HSV)
    
    # HSV範囲で青色を抽出し、有無を返却
    mask = cv2.inRange(hsv_image, lower_hsv, upper_hsv)
    return np.any(mask > 0)

def get_target_circle_center(image):
    """ ターゲットサークルの中心座標取得

    Args:
        image (image): 撮影画像

    Returns:
        tuple: ターゲットサークルの中心座標(x, y)
    """
    
    # 青色のHSV値でマスクを生成(青色とみなす上限下限の範囲を指定)
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    lower_blue = np.array([100, 120, 30])
    upper_blue = np.array([125, 255, 180])
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    # 青色部分とそれ以外を分割
    blue_area = cv2.bitwise_and(image, image, mask=mask_blue)
    
    # RGB値に変換して出力
    blue_rgb = cv2.cvtColor(blue_area, cv2.COLOR_BGR2RGB)
    cv2.imwrite(f"/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/3_rgb{static_count}.png", blue_rgb)
    
    # 画像の縦横サイズを取得
    height, width = image.shape[:2]
    height_threshold = int(height * 0.2)

    # 青色部の輪郭を検出
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (3, 3), 0, 5)
    edges = cv2.Canny(blurred, 50, 150)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # 検出された輪郭の件数分繰り返し
    for cnt in contours:
        
        # 点の少ない輪郭は除外        
        if len(cnt) < 5:
            continue

        # 横長でないものは除外
        x, y, w, h = cv2.boundingRect(cnt)
        aspect_ratio_hv = h / float(w)
        aspect_ratio_vh = w / float(h)
        if aspect_ratio_hv > 0.8 or aspect_ratio_vh > 3.5:
            continue

        # 輪郭に楕円をフィッティング
        ellipse = cv2.fitEllipse(cnt)

        # 中心のx座標と座標を取得
        center_x = round(ellipse[0][0])        
        center_y = round(ellipse[0][1])
        
        # 上部にある輪郭は除外
        if center_y < height_threshold:
            continue

        # 付近に別の青色がある場合は除外
        if has_blue_nearby(image, ellipse):
            continue
        
        # 除外されなかった輪郭の中心をターゲットサークルの中心として返却
        return center_x, center_y

    # 対象なしの場合は0を返す
    return 0, 0

def is_target_circle_in_display(image):
    target_x, target_y = get_target_circle_center(image)
    return target_x != 0 and target_y != 0

static_count = 0

def get_blue_bottle_center(image):
    global static_count
    static_count += 1
    image2 = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    cv2.imwrite(f"/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/3_rgb{static_count}.png", image2)

    image3 = cv2.GaussianBlur(image2, (5, 5), 0)
    cv2.imwrite(f"/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/3_blur{static_count}.png", image3)

    image4 = cv2.inRange(image3, (95, 50, 50), (125, 255, 255))
    # image4 = cv2.inRange(image3, (85, 50, 50), (105, 255, 255)) # 自宅の光源だとこちらの方が良かった、背景にオレンジのマットを展開した
    cv2.imwrite(f"/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/3_binary{static_count}.png", image4)

    contours, _ = cv2.findContours(image4, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # 輪郭が見つからない場合は早期リターン
    if len(contours) == 0:
        return 0, 0
    
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
        cv2.circle(image, (x, y), 5, (255, 255, 127), 2)
        cv2.imwrite(f"/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/3_bottle{static_count}.png", image)
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



            # 分析コマンドがない場合はスキップ
            if (command == ANALYSIS_COMMAND.do_nothing.value):
                # 他のプロセスが読み取りを終えるまで待つ（セマフォをロック）
                sem_analysis_result.acquire()
                # 共有メモリを0でクリア
                # !!!!フォーマットが変わったときここも変更すること
                shm_analysis_result_map[:] = struct.pack(format_string, 0, 0, 0, 0, 0)
                sem_analysis_result.release()
                # 書き込みが完了したらセマフォをアンロック
                time.sleep(0.05)
                continue

            print("IAnaS: 分析", command, "を実行しています")
            
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
            time.sleep(0.05) # 50msごとに画像を分析

    finally:
        shm_image_map.close()
        shm_image.close_fd()
        shm_image.unlink() # 終了時に共有メモリを削除
        sem_image.close()
        sem_image.unlink() # 終了時にセマフォを削除


if __name__ == "__main__":
    main()
