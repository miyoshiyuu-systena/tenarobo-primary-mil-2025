#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include "config.h"

/**
 * ログレベル列挙型
 */
enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

/**
 * ログエントリ構造体
 */
struct LogEntry {
    std::time_t timestamp;
    LogLevel level;
    std::string message;
    
    LogEntry(LogLevel lvl, const std::string& msg) 
        : timestamp(std::time(nullptr)), level(lvl), message(msg) {}
};

/**
 * シングルトンロガークラス
 * ロボット制御中のログを収集し、プログラム終了時にファイルに出力する
 */
class Logger {
public:
    /**
     * シングルトンインスタンス取得
     * @return ロガーインスタンス
     */
    static Logger& getInstance();

    /**
     * INFOレベルのログを記録
     * @param message ログメッセージ
     */
    void logInfo(const std::string& message);

    /**
     * INFOレベルのログを記録
     * コンソールには出力しない
     * @param message ログメッセージ
     */
    void logInfoWithoutConsoleLog(const std::string& message);

    /**
     * WARNINGレベルのログを記録
     * @param message ログメッセージ
     */
    void logWarning(const std::string& message);

    /**
     * ERRORレベルのログを記録
     * @param message ログメッセージ
     */
    void logError(const std::string& message);

    /**
     * DEBUGレベルのログを記録
     * @param message ログメッセージ
     */
    void logDebug(const std::string& message);

    /**
     * ログエントリを追加
     * @param level ログレベル
     * @param message ログメッセージ
     * @param is_console_log コンソールに出力するかどうか
     */
    void addLogEntry(LogLevel level, const std::string& message, bool is_console_log);

    /**
     * ログをファイルに出力
     * ファイル名: yyyymmdd-hhmmss-log.txt
     */
    void writeLogsToFile();

    /**
     * デストラクタ - プログラム終了時にログファイルを出力
     */
    ~Logger();

    /**
     * 設定を再読み込みして、ログ設定を更新
     */
    void reloadConfig();

private:
    /**
     * プライベートコンストラクタ（シングルトンパターン）
     */
    Logger();

    /**
     * コピーコンストラクタを削除（シングルトンパターン）
     */
    Logger(const Logger&) = delete;

    /**
     * 代入演算子を削除（シングルトンパターン）
     */
    Logger& operator=(const Logger&) = delete;

    /**
     * ログディレクトリを作成
     */
    void createLogDirectory();

    /**
     * ファイル名を決定する
     * @return ファイル名（[カウント]-[サフィックス]-log.txt）
     */
    std::string generateLogFileName();

    /**
     * ログレベルを文字列に変換
     * @param level ログレベル
     * @return ログレベル文字列
     */
    std::string logLevelToString(LogLevel level) const;

    /**
     * 時間を文字列に変換
     * @param timestamp タイムスタンプ
     * @return 時間文字列
     */
    std::string timeToString(std::time_t timestamp) const;

    std::vector<LogEntry> m_logEntries;  ///< ログエントリのリスト
    std::string m_logDirectory;          ///< ログファイル出力ディレクトリ
    std::string m_logFileNameSuffix;     ///< ログファイル名サフィックス
    int m_logFileCount;                  ///< ログファイルのカウント
};

#endif // LOGGER_H 