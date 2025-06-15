#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include "spdlog/spdlog.h" // Spdlogのヘッダファイルをインクルード
#include "spdlog/sinks/daily_file_sink.h" // 日次ファイルローテーション用
#include "spdlog/sinks/stdout_color_sinks.h" // コンソール出力用

class Logger {
public:
    // シングルトンインスタンスを取得
    static Logger& getInstance();

    // デストラクタ (シングルトンのためprotected/privateではない)
    ~Logger();

    // ログレベルを設定
    void setLogLevel(spdlog::level::level_enum level);

    // 各ログレベルでのログ出力関数
    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

private:
    // コンストラクタ (シングルトンパターンによりprivate)
    Logger();

    // コピーコンストラクタと代入演算子を禁止 (シングルトンパターン)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::shared_ptr<spdlog::logger> spdLogger;
};

#ifdef __cpluspluc
}
#endif

#endif // LOGGER_H