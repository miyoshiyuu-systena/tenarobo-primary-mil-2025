#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <fstream>
#include <map>
#include <iostream>

/**
 * 実行時の設定を外部ファイルから読み込むクラス
 */
class ConfigLoader {
private:
    std::map<std::string, std::string> configValues;
    std::string configFilePath;
    
    // デフォルト値
    const std::string DEFAULT_LOG_PATH = "/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/log/";
    const std::string DEFAULT_LOG_SUFFIX = "";
    const std::string DEFAULT_IMG_PATH = "/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/";
    
    /**
     * 設定ファイルを解析する
     */
    bool parseConfigFile() {
        std::ifstream file(configFilePath);
        if (!file.is_open()) {
            std::cerr << "設定ファイルが開けません: " << configFilePath << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // コメント行をスキップ
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // key=value形式を解析
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                
                // 前後の空白を削除
                key = trim(key);
                value = trim(value);
                
                configValues[key] = value;
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * 文字列の前後の空白を削除
     */
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) return "";
        
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
public:
    /**
     * コンストラクタ
     * @param configPath 設定ファイルのパス
     */
    ConfigLoader(const std::string& configPath = "settings.conf") 
        : configFilePath(configPath) {
        loadConfig();
    }
    
    /**
     * 設定ファイルを読み込む
     */
    bool loadConfig() {
        return parseConfigFile();
    }
    
    /**
     * 設定値を取得する
     * @param key キー
     * @param defaultValue デフォルト値
     * @return 設定値
     */
    std::string getValue(const std::string& key, const std::string& defaultValue = "") {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    /**
     * ログファイルのパスを取得
     */
    std::string getLogFilePath() {
        return getValue("logFilePath", DEFAULT_LOG_PATH);
    }
    
    /**
     * ログファイル名のサフィックスを取得
     */
    std::string getLogFileNameSuffix() {
        return getValue("logFileNameSuffix", DEFAULT_LOG_SUFFIX);
    }
    
    /**
     * 画像ファイルのパスを取得
     */
    std::string getImgFilePath() {
        return getValue("imgFilePath", DEFAULT_IMG_PATH);
    }
    
    /**
     * 画像ファイル名のサフィックスを取得
     */
    std::string getImgFileNameSuffix() {
        std::string suffix = getValue("imgFileNameSuffix", "");
        // 空の場合はlogFileNameSuffixと同じ値を使用
        if (suffix.empty()) {
            suffix = getLogFileNameSuffix();
        }
        return suffix;
    }
    
    /**
     * 設定値を表示（デバッグ用）
     */
    void printConfig() {
        std::cout << "=== 設定情報 ===" << std::endl;
        std::cout << "logFilePath: " << getLogFilePath() << std::endl;
        std::cout << "logFileNameSuffix: " << getLogFileNameSuffix() << std::endl;
        std::cout << "imgFilePath: " << getImgFilePath() << std::endl;
        std::cout << "imgFileNameSuffix: " << getImgFileNameSuffix() << std::endl;
    }
};

// グローバルインスタンス
extern ConfigLoader config;

#endif // _CONFIG_H_