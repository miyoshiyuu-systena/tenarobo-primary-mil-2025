# ET ロボコン 2025 に向けた制御ソフトの開発

目的は、技術学習とする。

したがって、単にプログラム改善するだけでなく、設計とプロジェクトマネジメントの結果もpushし、その向上の過程をも記録する。

# 構成

| ディレクトリ/ファイル | 役割 |
|--|--|
| `action/` | ロボットの行動制御（直進、カーブ、停止など）を実装するアクションクラス群 |
| `app.cfg` | TOPPERS/ASP3のためのタスク定義ファイル |
| `app.cpp` | メインプログラムの実装 |
| `app.h` | メインメソッドの定義ヘッダーファイル |
| `asset/` | プロジェクトで使用する画像やドキュメント素材 |
| `assist/` | ライントレースや加速制御などの補助機能を提供するクラス群 |
| `boot/` | RaspberryPi起動時の自動実行設定とスクリプト |
| `closer/` | 終了条件を判定するクラス群（時間、色、カーブ検出など） |
| `CONFIG_USAGE.md` | 設定ファイルの使用方法に関するドキュメント |
| `config.cpp` | 設定管理の実装 |
| `config.h` | 設定管理のヘッダーファイル |
| `dev/` | 開発環境セットアップの手引き |
| `device/` | ハードウェアデバイス制御の抽象化レイヤー |
| `drive/` | モーター制御やドライブ機構の実装 |
| `logger/` | ログ出力機能の実装 |
| `Makefile.inc` | ビルド設定のインクルードファイル |
| `pj/` | プロジェクト管理とIssue対応に関するドキュメント |
| `README.md` | 本ドキュメント |
| `settings.conf` | アプリケーション設定ファイル |
| `web-camera/` | Webカメラ制御とOpenCV連携機能 |

# 関連

| No | 名前 | リンク |
| -- | -- | -- |
| 1 | ETロボコンHP | https://www.etrobo.jp/ |
| 2 | ET ロボコン提供開発環境 | https://github.com/ETrobocon/RasPike-ART |
| 3 | RasPI開発環境セットアップ | [dev/onRasPI.md](dev/onRasPI.md) |
| 4 | myPC開発環境セットアップ | [dev/README.md](dev/README.md) |
| 5 | RasPI起動時プログラム実行 | [boot/README.md](boot/README.md) |
