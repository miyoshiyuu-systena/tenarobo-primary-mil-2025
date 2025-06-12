#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

const std::string LOG_BASE_DIR = "work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/log";

/**
 * ログを書き込む
 * @param ログメッセージ
 * @return ログの書込み是非
 */
bool writelog(const std::string& logMessage) {
    // 1. ログファイルのファイル名を決定
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_c);

    std::stringstream date_ss;
    date_ss << std::put_time(local_tm, "%Y-%m-%d");

    std::string current_log_filename = date_ss.str() + ".log";

    // 2. ログファイルのフルパスを取得
    std::filesystem::path home_dir;
    const char* home_env = std::getenv("HOME"); // Linux/macOS
    if (home_env) {
        home_dir = home_env;
    } else {
        syslog(LOG_ERR, "[ERR]HOME環境変数を取得できませんでした。");
        return false;
    }
    std::filesystem::path full_log_folder_path = home_dir / LOG_BASE_DIR;

    // 3. フォルダの存在確認と作成
    if (
        (!std::filesystem::exists(full_log_folder_path)) &&
        (!std::filesystem::create_directories(full_log_folder_path))
    ) {
        syslog(LOG_ERROR, "[ERR]ログファイルを作成できませんでした。");
        return false;
    }

    // 4. フルパスの構築
    std::filesystem::path full_log_file_path = full_log_folder_path / current_log_filename;

    // 5. ファイルを開く
    // std::ios::app は追記モード (ファイルが存在すれば末尾に追記、なければ新規作成)
    std::ofstream outputFile(fullPath, std::ios::app);

    // ファイルが開けたか確認
    if (!outputFile.is_open()) {
        syslog(LOG_ERROR, "[ERR]ファイルを開けませんでした。");
        return false;
    }

    // 6. ログを書き込む
    std::stringstream formatted_log_ss;
    formatted_log_ss << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << " " << logMessage;
    outputFile << formatted_log_ss.str() << std::endl; // 改行も追加

    // 7. ファイルを閉じる
    outputFile.close();

    return true;
}