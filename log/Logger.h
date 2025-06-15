#include <spdlog/spdlog.h>
#include <string>
#include <memory>

class Logger {
public:
    /**
     * シングルトンインスタンスの取得
     */
    static Logger& getInstance();

    /**
     * spdロガーインスタンスの取得
     */
    std::shared_ptr<spdlog::logger>& getSpdLogger();

    void end();

    /**
     * コピーコンストラクタと代入演算子を削除し、シングルトンを強制
     */
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    /**
     * コンストラクタ
     * 
     * シングルトンクラスではインスタンス化で量産されないようにするため、
     * コンストラクタをプライベートにする。
     */
    Logger();

    /**
     * ログファイルの名前を生成する
     */
    std::string generateLogFilename();

    /**
     * ロガーのインスタンス
     */
    std::shared_ptr<spdlog::logger> m_logger;
};