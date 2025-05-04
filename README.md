# tenarobo-primary-mil-2025

ET ロボコン 2025 に向けた制御ソフトの開発

- [ET ロボコン HP](https://www.etrobo.jp/)

- [ET ロボコン提供開発環境](https://github.com/ETrobocon/RasPike-ART)

# 目的

mada

# 開発

## 実機

- Raspberry PI

- SPIKE（Lego）

- 各種 PUP モジュール（Lego）（モーターやカラーセンサ）

- Web カメラ

## OS 環境

Raspberry PI OS(64bit)にて動作する。

OS をインストールするには Raspberry PI 本機の他に、インターネットに接続できる PC 、OS を保存する SD カード（Raspberry PI に挿入する）が必要である。

### 1. OS イメージのダウンロード

64bit 版 OS イメージを公式 HP からダウンロードする。

ダウンロードしたそのもの SD カードに保存するのではダメで、Imager などの"OS 焼きこみソフト"を使用して SD カードから立ち上げられるようにしなければならない。

- [公式 HP](https://www.raspberrypi.com/software/operating-systems/)

- [Raspberry PI Imager](https://www.raspberrypi.com/software/)

### 2. Raspberry PI の起動

PI 本機の電源を落とした状態で、背面のポートに SD カードを挿入する。

PI 本機の電源を付けたときに、OS のエントリー画面が出てくれば成功。（next ボタンがある）

### 3. 初期セットアップ

言語・アカウント・ネットワークなどを設定する。

### 4. 確認

以下のコマンドを実行する。

```
uname -a
```

以下の出力が得られた場合、OS は正しい。

```
Linux raspberrypi 6.6.51+rpt-rpi-v8 #1 SMP PREEMPT Debian 1:6.6.51-1+rpt3 (2024-10-08) aarch64 GNU/Linux
```

さらに、以下のコマンドを実行する。

```
uname -m
```

以下の出力が得られた場合、ユーザーランドは正しい。

```
aarch64
```

ユーザーランドが 64bit 形式のものでない場合、RasPike-ART が動作しない。

もう一度 OS イメージのクリーンインストールをしてみる。

## 開発手順

以下に、Raspberry PI に本プロジェクトをダウンロード・コンパイル・実行（試験運転）するまでの手順を示す。

### 1. RasPike-ART プロジェクトのダウンロード

本プロジェクトは、RasPike-ART プロジェクトを開発基礎とする。（先行開発者が実装した変数やメソッドを再利用する）

そのため、RasPike-ART プロジェクトを一度ダウンロードして、その中に本プロジェクトを内包させる。

Raspberry PI にて ctrl + alt + t を入力し、ターミナルを起動する。

ターミナルから適当なフォルダに移動して（ls、pwd、cd フォルダ名、といったコマンドを駆使する）、以下のコマンドを実行する。

```
git clone --recursive https://github.com/ETrobocon/RasPike-ART.git
```

> 失敗した場合、インターネットに接続できていないかもしれない。
>
> Raspberry PI のインターネット設定を確認し、有線ケーブルや Wifi でインターネットに接続する。

### 2. SPIKE への書き込み

SPIKE と Raspberry PI を USB ケーブルで接続する。

引き続きターミナルにて、以下の 2 コマンドを実行する。

```
cd RasPike-ART/sdk/workspace
make -f ../common/Makefile.raspike-art setup_spike_env
```

これに SPIKE へ書き込むための python の仮想環境が作成される。（コマンドの終了に若干時間がかかるはず。）

ここで SPIKE の電源を落とし（メインボタン長押しして、四角形マークが出るまで）、USB ケーブルを引き抜く。

さらに、SPIKE の bluetooth ボタンを長押ししたまま、USB ケーブルを再度挿入する（ボタンの周りがカラフルに光りだすまで押し続ける）。

- [DFU モード](https://afrel.co.jp/product/spike/technology-spike/basic/software-basic/54122/)

この状態で、以下のコマンドを実行する。

```
make -f ../common/Makefile.raspike-art update_spike
```

以下の表示が出たら書き込み完了。

```
Writing memory...
0x08008000  130600 [=========================] 100%
Exiting DFU...
Finished
+ rm -rf firmware.dfu
```

### 3. 本プロジェクトのダウンロード

引き続きターミナルにて、以下のコマンドを実行する。（ここはシステナオリジナル）

```
git clone https://github.com/miyoshiyuu-systena/tenarobo-primary-mil-2025
```

ls コマンドで確認すると、tenarobo-primary-mil-2025 というフォルダが発生する。

- 基本的には、Raspberry PI 内ではなく、他の PC からソースコードを編集する

- 編集はリモートリポジトリに push し、Raspberry PI から最新や任意のブランチを pull することで動作確認用のソースコードを入れ替える

### 4. Raspberry PI 側のソースコードのコンパイル

引き続きターミナルにて、以下のコマンドを実行する。

```
make img=tenarobo-primary-mil-2025
```

これでプロジェクトが機械語に翻訳され、全てのファイル（RasPike-ART を含む）が結合される。

また、プログラム実行のターゲットも"tenarobo-primary-mil-2025"となるので、次のコマンドではフォルダ名を指定しなくてもよい。

### 5. プログラム実行

SPIKE のメインボタンを長押しして、一度電源を切る。

再度メインボタンを押して、電源を付ける（∞ マークになる）

その状態で、引き続きターミナルにて、以下のコマンドを実行する。

```
make start
```

初期通信直後、一瞬 SPIKE に「ET」の文字が現れて、プログラムが開始する。

### 6. プログラム停止

ターミナルにて、ctrl + c でプログラムを停止する。

SPIKE のメインボタンを長押しして、電源を切る。

（再度実行するときは、SPIKE の電源を ON にして、make start）

順番がずれると接続がうまくいかないことがある。

> なお、イメージターゲットを"sample_c5_spike"とすると、同フォルダにあるサンプルプロジェクトがコンパイルされる。
>
> ソースコードを観察すると、ポート位置と PUP デバイスの対応関係が記述してあるので、その通りに接続し 4., 5.の操作をすると、モーターの回転を確認できる。
>
> なおちょっとでもデバイスの接続位置が間違っていると、全く動かない。

## 構成

```
doc
┗　design
┗　course-analysis
app.cpp
app.hpp
app.cfg
README.md
```

# 動作確認

## システムサービスによる起動時メイン実行

mada

## ログ

mada
