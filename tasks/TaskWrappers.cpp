#include "app.h"
#include "ImageAcquisitionTask.h"

/**
 * 画像取得タスクのラッパー関数
 * TOPPERS/ASP3のC環境からC++クラスメソッドを呼び出す
 */
extern "C" void image_acquisition_task(intptr_t exinf) {
    ImageAcquisitionTask::run(exinf);
}
