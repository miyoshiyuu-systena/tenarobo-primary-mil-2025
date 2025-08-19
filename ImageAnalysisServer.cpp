#include "ImageAnalysisServer.h"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

/**
 * 分析コマンドの共有メモリの名前
 * プロセス間で名前を統一する必要がある
 */
const char* SHM_ANALYSIS_COMMAND_NAME = "/robot_shm_analysis_command";
const char* SEM_ANALYSIS_COMMAND_NAME = "/robot_sem_analysis_command";

static int shm_analysis_command_fd = -1;
static void* shm_analysis_command_ptr = NULL;
static sem_t* sem_analysis_command = SEM_FAILED;

/**
 * 分析結果の共有メモリの名前
 * プロセス間で名前を統一する必要がある
 */
const char* SHM_ANALYSIS_RESULT_NAME = "/robot_shm_analysis_result";
const char* SEM_ANALYSIS_RESULT_NAME = "/robot_sem_analysis_result";

static int shm_analysis_result_fd = -1;
static void* shm_analysis_result_ptr = NULL;
static sem_t* sem_analysis_result = SEM_FAILED;

static bool is_prepared = false;

static void signal_handler(int signal) {
    if (signal == SIGINT) {
        ImageAnalysisServer::getInstance().cleanup();
        exit(0);
    }
}

ImageAnalysisServer& ImageAnalysisServer::getInstance() {
    static ImageAnalysisServer instance;
    return instance;
}

ImageAnalysisServer::ImageAnalysisServer() {
    // シグナルハンドラの設定
    signal(SIGINT, signal_handler);

    /**
     * 分析コマンドの共有メモリの取得
     * 「取得」なので、コンピュータの中にメモリが作成されていない場合はエラーになる
     * つまり、Python画像分析サーバーを先に立ち上げる必要があることに注意
     */
    shm_analysis_command_fd = shm_open(SHM_ANALYSIS_COMMAND_NAME, O_RDWR, 0666);
    if (shm_analysis_command_fd < 0) {
        std::cerr << "エラー: 分析コマンドの共有メモリを開けませんでした" << std::endl;
        return;
    }

    shm_analysis_command_ptr = mmap(NULL, sizeof(AnalysisCommand), PROT_READ | PROT_WRITE, MAP_SHARED, shm_analysis_command_fd, 0);
    if (shm_analysis_command_ptr == MAP_FAILED) {
        std::cerr << "エラー: 分析コマンドのメモリマップに失敗しました" << std::endl;
        close(shm_analysis_command_fd);
        return;
    }

    sem_analysis_command = sem_open(SEM_ANALYSIS_COMMAND_NAME, 0);
    if (sem_analysis_command == SEM_FAILED) {
        std::cerr << "エラー: 分析コマンドのセマフォを開けませんでした" << std::endl;
        munmap(shm_analysis_command_ptr, sizeof(AnalysisCommand));
        close(shm_analysis_command_fd);
        return;
    }

    /**
     * 分析結果の共有メモリの取得
     * 「取得」なので、コンピュータの中にメモリが作成されていない場合はエラーになる
     * つまり、Python画像分析サーバーを先に立ち上げる必要があることに注意
     */
    shm_analysis_result_fd = shm_open(SHM_ANALYSIS_RESULT_NAME, O_RDWR, 0666);
    if (shm_analysis_result_fd < 0) {
        std::cerr << "エラー: 分析結果の共有メモリを開けませんでした" << std::endl;
        return;
    }

    shm_analysis_result_ptr = mmap(NULL, sizeof(AnalysisResultPacket), PROT_READ | PROT_WRITE, MAP_SHARED, shm_analysis_result_fd, 0);
    if (shm_analysis_result_ptr == MAP_FAILED) {
        std::cerr << "エラー: 分析結果のメモリマップに失敗しました" << std::endl;
        close(shm_analysis_result_fd);
        return;
    }

    sem_analysis_result = sem_open(SEM_ANALYSIS_RESULT_NAME, 0);
    if (sem_analysis_result == SEM_FAILED) {
        std::cerr << "エラー: 分析結果のセマフォを開けませんでした" << std::endl;
        munmap(shm_analysis_result_ptr, sizeof(AnalysisResultPacket));
        close(shm_analysis_result_fd);
        return;
    }

    is_prepared = true;
}

void ImageAnalysisServer::cleanup() {
    if (is_prepared) {
        munmap(shm_analysis_command_ptr, sizeof(AnalysisCommand));
        close(shm_analysis_command_fd);
        sem_close(sem_analysis_command);
        is_prepared = false;
    }
}

ImageAnalysisServer::~ImageAnalysisServer() {
}

void ImageAnalysisServer::requestDoNothing() {
    // セマフォをロック
    sem_wait(sem_analysis_command);

    AnalysisCommand* shared_command = static_cast<AnalysisCommand*>(shm_analysis_command_ptr);
    *shared_command = DO_NOTHING;

    sem_post(sem_analysis_command);
    // セマフォをアンロック
}

void ImageAnalysisServer::request(enum AnalysisCommand command) {
    // セマフォをロック
    sem_wait(sem_analysis_command);

    AnalysisCommand* shared_command = static_cast<AnalysisCommand*>(shm_analysis_command_ptr);
    *shared_command = command;

    sem_post(sem_analysis_command);
    // セマフォをアンロック
}

bool ImageAnalysisServer::responseFrontStraight() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->front_straight;
    sem_post(sem_analysis_result);
    return result;
}

bool ImageAnalysisServer::responseTargetCircleInDisplay() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->target_circle_in_display;
    sem_post(sem_analysis_result);
    return result;
}

bool ImageAnalysisServer::responseOnLeftEdge() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->on_left_edge;
    sem_post(sem_analysis_result);
    return result;
}

bool ImageAnalysisServer::responseGateInFront() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->gate_in_front;
    sem_post(sem_analysis_result);
    return result;
}

bool ImageAnalysisServer::responseRedBottleInFront() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->red_bottle_in_front;
    sem_post(sem_analysis_result);
    return result;
}

bool ImageAnalysisServer::responseBlueBottleInFront() {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    bool result = shared_result->blue_bottle_in_front;
    sem_post(sem_analysis_result);
    return result;
}

void ImageAnalysisServer::responseTargetCircleXY(uint16_t& x, uint16_t& y) {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    x = shared_result->target_circle_x;
    y = shared_result->target_circle_y;
    sem_post(sem_analysis_result);
}

void ImageAnalysisServer::responseBlueBottleXY(uint16_t& x, uint16_t& y) {
    sem_wait(sem_analysis_result);
    AnalysisResultPacket* shared_result = static_cast<AnalysisResultPacket*>(shm_analysis_result_ptr);
    x = shared_result->blue_bottle_x;
    y = shared_result->blue_bottle_y;
    sem_post(sem_analysis_result);
}

void ImageAnalysisServer::responseBlueBottleXYStatic(uint16_t& x, uint16_t& y) {
    ImageAnalysisServer::getInstance().responseBlueBottleXY(x, y);
}