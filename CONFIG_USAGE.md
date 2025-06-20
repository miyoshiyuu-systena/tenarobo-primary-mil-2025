# 設定ファイルローダーの使用方法

## 概要
`config.h` に含まれる `ConfigLoader` クラスは、実行時に外部の設定ファイルから設定値を読み込む機能を提供します。

## ファイル構成
- `config.h` - ConfigLoaderクラスの定義
- `config.cpp` - ConfigLoaderのグローバルインスタンス定義
- `settings.conf` - 設定ファイル（サンプル）
- `config_test.cpp` - 使用例とテストプログラム

## 設定ファイル形式
設定ファイルは `key=value` 形式で記述します：

```
# ログファイルの出力パス
logFilePath=/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/log/

# ログファイル名のサフィックス
logFileNameSuffix=_debug

# 画像ファイルの出力パス
imgFilePath=/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/

# 画像ファイル名のサフィックス（空の場合はlogFileNameSuffixと同じ値）
imgFileNameSuffix=
```

### 注意事項
- `#` または `;` で始まる行はコメントとして無視されます
- 空行は無視されます
- 前後の空白は自動的に削除されます

## 使用方法

### 1. 基本的な使用方法
```cpp
#include "config.h"

int main() {
    // グローバルインスタンス config を使用
    std::string logPath = config.getLogFilePath();
    std::string imgPath = config.getImgFilePath();
    
    std::cout << "ログパス: " << logPath << std::endl;
    std::cout << "画像パス: " << imgPath << std::endl;
    
    return 0;
}
```

### 2. 利用可能なメソッド
- `getLogFilePath()` - ログファイルのパスを取得
- `getLogFileNameSuffix()` - ログファイル名のサフィックスを取得
- `getImgFilePath()` - 画像ファイルのパスを取得
- `getImgFileNameSuffix()` - 画像ファイル名のサフィックスを取得
- `getValue(key, defaultValue)` - 任意のキーの値を取得
- `printConfig()` - 全設定値をコンソールに表示（デバッグ用）

### 3. カスタム設定値の追加
設定ファイルに独自のキーを追加し、`getValue()` メソッドで取得できます：

設定ファイル：
```
# カスタム設定
debugMode=true
maxRetryCount=3
```

C++コード：
```cpp
bool debugMode = (config.getValue("debugMode", "false") == "true");
int maxRetryCount = std::stoi(config.getValue("maxRetryCount", "1"));
```

## テスト実行
```bash
# コンパイル
g++ -o config_test config_test.cpp config.cpp

# 実行
./config_test
```

## 設定ファイルのパス変更
デフォルトでは `settings.conf` を読み込みますが、異なるパスを指定する場合は：

```cpp
ConfigLoader customConfig("path/to/your/config.conf");
```

## デフォルト値
設定ファイルが見つからない場合や、キーが存在しない場合は、以下のデフォルト値が使用されます：
- `logFilePath`: `/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/log/`
- `logFileNameSuffix`: 空文字列
- `imgFilePath`: `/home/mil/work/RasPike-ART/sdk/workspace/tenarobo-primary-mil-2025/img-debug/`
- `imgFileNameSuffix`: `logFileNameSuffix` と同じ値