#include    "CameraTask.h"
#include    "CameraManager.h"
#include    "logger/Logger.h"

void camera_task(intptr_t exinf) {
    CameraManager& cameraManager = CameraManager::getInstance();
    if (cameraManager.isInitialized() == false) {
        cameraManager.initializeCamera();
    }

    if (cameraManager.captureImageNow() == false) {
        Logger::getInstance().logError("カメラの画像取得に失敗しました");
    } else {
        Logger::getInstance().logInfo("カメラの画像取得に成功しました");
    }

    ext_tsk();
}