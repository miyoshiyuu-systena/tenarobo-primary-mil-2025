#include    "Logger.h"
#include    "syssvc/syslog.h"
#include    <spdlog/sinks/basic_file_sink.h> // ファイルシンク用
#include    <chrono>   // 時間取得用
#include    <iomanip>  // std::put_time 用
#include    <sstream>  // std::stringstream 用
#include    <ctime>    // std::localtime 用

// シングルトンインスタンスの静的メンバー変数の定義
// プログラム起動時に一度だけ初期化されます
Logger& Logger::getInstance() {
    static Logger instance; // 関数スコープの static 変数でシングルトンを実現
    return instance;
}

// プライベートコンストラクタの実装
Logger::Logger() {
    try {
        // ログファイル名を生成
        std::string log_filename = generateLogFilename();

        // ファイルロガーを初期化
        // ロガー名 "my_application_logger"
        // ファイル名: 生成したファイル名 (例: logs/2025-06-15 16:15:21.0123.log)
        // true: 既存のファイルを上書き (これは初回なので常に新しいファイル)
        m_logger = spdlog::basic_logger_mt("my_application_logger", log_filename, true);

        // ログレベルの設定 (例: INFO 以上のメッセージを出力)
        m_logger->set_level(spdlog::level::trace); // デバッグしやすいようにTRACEに設定

        // ログパターンの設定
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%L] [%n] %v");

        // ロガーが作成されたことを確認するための内部ログ (必要であれば)
        // spdlog::info("Logger initialized successfully. Log file: {}", log_filename);

    } catch (const spdlog::spdlog_ex& ex) {
        // ロガー初期化中にエラーが発生した場合
        // ここでは標準エラー出力に直接メッセージを出力します
        // spdlog はまだ使えないため
        syslog(LOG_ERROR, "Logger initialization failed: ");
        // プログラムを続行しない場合は、ここで例外を再スローするか、終了する
        throw; // 初期化失敗は通常致命的
    }
}

// 内部の spdlog ロガーへの参照を取得するメソッド
std::shared_ptr<spdlog::logger>& Logger::getSpdLogger() {
    return m_logger;
}

// ログファイル名を生成するプライベートメソッド
std::string Logger::generateLogFilename() {
    // 現在の時間を取得
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* timeinfo = std::localtime(&now_c);

    // 日時フォーマット
    std::stringstream ss;
    ss << std::put_time(timeinfo, "%Y-%m-%d %H-%M-%S"); // ファイル名に ':' は使えないため '-' に変更
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count(); // ミリ秒を追加

    // ログファイルのディレクトリとファイル名を結合
    return "logs/" + ss.str() + ".log";
}