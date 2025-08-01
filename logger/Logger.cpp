#include "Logger.h"
#include "../config.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>

/**
 * シングルトンインスタンス取得
 */
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

/**
 * プライベートコンストラクタ
 */
Logger::Logger() {
    // config.hの設定を使用
    m_logDirectory = logFilePath;
    m_logFileNameSuffix = logFileNameSuffix;
    m_logFileCount = 0;

    // ログディレクトリの存在確認と作成
    createLogDirectory();
    
    std::cout << "ログディレクトリ: " << m_logDirectory << std::endl;
}

/**
 * ログディレクトリを作成
 */
void Logger::createLogDirectory() {
    struct stat st = {0};
    
    if (stat(m_logDirectory.c_str(), &st) == -1) {
        if (mkdir(m_logDirectory.c_str(), 0755) == 0) {
            std::cout << "ログディレクトリを作成しました: " << m_logDirectory << std::endl;
        } else {
            std::cerr << "ログディレクトリの作成に失敗しました: " << m_logDirectory << std::endl;
        }
    }
}

/**
 * デストラクタ - プログラム終了時にログファイルを出力
 */
Logger::~Logger() {
    writeLogsToFile();
}

/**
 * INFOレベルのログを記録
 */
void Logger::logInfo(const std::string& message) {
    addLogEntry(LogLevel::INFO, message, true);
}

/**
 * INFOレベルのログを記録
 * コンソールには出力しない
 * @param message ログメッセージ
 */
void Logger::logInfoWithoutConsoleLog(const std::string& message) {
    addLogEntry(LogLevel::INFO, message, false);
}

/**
 * WARNINGレベルのログを記録
 */
void Logger::logWarning(const std::string& message) {
    addLogEntry(LogLevel::WARNING, message, true);
}

/**
 * ERRORレベルのログを記録
 */
void Logger::logError(const std::string& message) {
    addLogEntry(LogLevel::ERROR, message, true);
}

/**
 * DEBUGレベルのログを記録
 */
void Logger::logDebug(const std::string& message) {
    addLogEntry(LogLevel::DEBUG, message, true);
}

/**
 * ログエントリを追加
 * @param level ログレベル
 * @param message ログメッセージ
 * @param is_console_log コンソールに出力するかどうか
 */
void Logger::addLogEntry(LogLevel level, const std::string& message, bool is_console_log) {
    m_logEntries.emplace_back(level, message);
    
    // リアルタイムでコンソールにも出力（デバッグ用） 
    if (is_console_log) {
        std::cout << "[" << timeToString(std::time(nullptr)) << "] " 
                  << logLevelToString(level) << ": " << message << std::endl;
    }
    
    // 定期的にファイルに書き込み（100エントリごと）
    if (m_logEntries.size() % 100 == 0) {
        writeLogsToFile();
    }
}

/**
 * ログをファイルに出力
 */
void Logger::writeLogsToFile() {
    if (m_logEntries.empty()) {
        return;
    }
    
    std::string filename = m_logDirectory + generateLogFileName();
    std::ofstream logFile(filename, std::ios::app);  // 追記モード
    
    if (!logFile.is_open()) {
        std::cerr << "ログファイルの作成に失敗しました: " << filename << std::endl;
        std::cerr << "ログディレクトリの確認: " << m_logDirectory << std::endl;
        
        // 代替パス（現在のディレクトリ）を試す
        std::string altFilename = generateLogFileName();
        logFile.open(altFilename, std::ios::app);
        
        if (!logFile.is_open()) {
            std::cerr << "代替パスでもログファイルの作成に失敗しました: " << altFilename << std::endl;
            return;
        } else {
            std::cout << "代替パスでログファイルを作成しました: " << altFilename << std::endl;
            filename = altFilename;
        }
    }
    
    // ファイルが新規作成された場合のみヘッダーを書き込み
    logFile.seekp(0, std::ios::end);
    if (logFile.tellp() == 0) {
        // ログファイルのヘッダー
        logFile << "=== ロボット制御ログ ===" << std::endl;
        logFile << "開始時刻: " << timeToString(std::time(nullptr)) << std::endl;
        logFile << "========================" << std::endl << std::endl;
    }
    
    // ログエントリを出力
    for (const auto& entry : m_logEntries) {
        logFile << "[" << timeToString(entry.timestamp) << "] " 
                << logLevelToString(entry.level) << ": " 
                << entry.message << std::endl;
    }
    
    logFile.close();
    std::cout << "ログファイルを出力しました: " << filename << " (エントリ数: " << m_logEntries.size() << ")" << std::endl;
    
    // 書き込み完了後、メモリからエントリをクリア（メモリ使用量を抑制）
    m_logEntries.clear();
}

/**
 * ファイル名を決定する
 */
std::string Logger::generateLogFileName() {
    std::ostringstream oss;
    oss << m_logFileCount << "-" << m_logFileNameSuffix << "-log.txt";
    m_logFileCount++;
    return oss.str();
}

/**
 * ログレベルを文字列に変換
 */
std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

/**
 * 時間を文字列に変換
 */
std::string Logger::timeToString(std::time_t timestamp) const {
    std::tm* timeinfo = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
} 