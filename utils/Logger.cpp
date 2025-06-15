#include "Logger.h"
#include <iostream> // エラー出力用

// シングルトンインスタンスの初期化 (プログラム起動時に一度だけ実行される)
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// コンストラクタ
Logger::Logger() {
    try {
        // コンソールシンクの作成
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v"); // 色付き出力
        console_sink->set_level(spdlog::level::debug); // コンソールにはDEBUG以上を出力

        // 日次ファイルシンクの作成 (ログファイル名、時間、ファイル数)
        // daily_file_sink_mt("logs/application.log", 0, 0) は、
        // logsディレクトリにapplication.logという名前で、毎日0時0分に新しいファイルを作成します。
        // 例: application_2025-06-15.log
        auto daily_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/application.log", 0, 0);
        daily_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        daily_file_sink->set_level(spdlog::level::trace); // ファイルにはTRACE以上を出力

        // ロガーの作成 (複数のシンクを持つ)
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(console_sink);
        sinks.push_back(daily_file_sink);
        spdLogger = std::make_shared<spdlog::logger>("my_app_logger", begin(sinks), end(sinks));

        // グローバルなデフォルトロガーとして登録 (オプション)
        // spdlog::set_default_logger(spdLogger);
        
        // ロガーのレベル設定 (デフォルトはinfo)
        spdLogger->set_level(spdlog::level::info);
        spdLogger->flush_on(spdlog::level::warn); // WARN以上は即時フラッシュ
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        // ロガーの初期化に失敗した場合の処理 (アプリケーションの終了など)
    }
}

// デストラクタ
Logger::~Logger() {
    spdlog::drop_all(); // 全てのロガーを解放
}

// ログレベルを設定
void Logger::setLogLevel(spdlog::level::level_enum level) {
    if (spdLogger) {
        spdLogger->set_level(level);
    }
}

// 各ログレベルでのログ出力関数
void Logger::trace(const std::string& msg) {
    if (spdLogger) {
        spdLogger->trace(msg);
    }
}

void Logger::debug(const std::string& msg) {
    if (spdLogger) {
        spdLogger->debug(msg);
    }
}

void Logger::info(const std::string& msg) {
    if (spdLogger) {
        spdLogger->info(msg);
    }
}

void Logger::warn(const std::string& msg) {
    if (spdLogger) {
        spdLogger->warn(msg);
    }
}

void Logger::error(const std::string& msg) {
    if (spdLogger) {
        spdLogger->error(msg);
    }
}

void Logger::critical(const std::string& msg) {
    if (spdLogger) {
        spdLogger->critical(msg);
    }
}