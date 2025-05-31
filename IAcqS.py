"""
IAcqS
Image Acquisition Server
ここでは画像取得を実行する
特定の名前のついた共有メモリに最新の画像を書き込む
"""

import cv2
import numpy as np
import posix_ipc
import mmap
import time

SHM_IMAGE_NAME = "/robot_shm_image"
SEM_IMAGE_NAME = "/robot_sem_image"

def main():
    acq_count = 0 # debug

    width = 320
    height = 240
    image_shape = (height, width, 3) # 画像サイズに合わせて変更
    image_size = np.prod(image_shape) * np.uint8().itemsize

    # POSIX共有メモリを作成
    shm = posix_ipc.SharedMemory(SHM_IMAGE_NAME, posix_ipc.O_CREAT, size=int(image_size))
    shm_map = mmap.mmap(shm.fd, shm.size)

    # セマフォの初期値を1に設定
    sem = posix_ipc.Semaphore(SEM_IMAGE_NAME, posix_ipc.O_CREAT, initial_value=1)

    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("IAcqS: エラー: カメラを開けませんでした")
            return
        
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

        print("IAcqS: バックグラウンドで画像を取得して、Raspberry Piの共有メモリに書き込んでいます。(30fps)")
        print("IAcqS: 終了するにはCtrl+Cを押してください。")
        while True:
            ret, frame = cap.read()
            if not ret:
                print("IAcqS: エラー: フレームを読み込めませんでした。")
                break
            
            if (acq_count % 30 == 0): # debug
                cv2.imwrite(f"env_image_{acq_count}.jpg", frame) # debug

            # 他のプロセスが読み取りを終えるまで待つ（セマフォをロック）
            sem.acquire()
            try:
                shm_map[:] = frame.tobytes()
            finally:
                # 書き込みが完了したらセマフォをアンロック
                sem.release()

            time.sleep(0.033) # 30fpsに相当するウェイト
            acq_count += 1 # debug

    finally:
        cap.release()
        shm_map.close()
        shm.close_fd()
        shm.unlink() # 終了時に共有メモリを削除
        sem.close()
        sem.unlink() # 終了時にセマフォを削除

if __name__ == "__main__":
    main()