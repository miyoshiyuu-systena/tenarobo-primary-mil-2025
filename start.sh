#!/bin/bash

# RasPike-ART 統合起動スクリプト
# 画像取得、画像処理、メインプログラムを順番に起動する

set -e  # エラーが発生した場合はスクリプトを終了

# 現在のディレクトリを保存
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$SCRIPT_DIR")"

echo "=== RasPike-ART 統合起動スクリプト開始 ==="
echo "スクリプトディレクトリ: $SCRIPT_DIR"
echo "ワークスペースディレクトリ: $WORKSPACE_DIR"

# 1. 画像取得プログラムを起動
echo ""
echo "=== 1. 画像取得プログラムを起動中... ==="
cd "$SCRIPT_DIR"

# 仮想環境をアクティベート
source venv/bin/activate

# 画像取得プログラムをバックグラウンドで実行
echo "画像取得プログラム (IAcqS.py) を起動..."
python IAcqS.py &
IMAGE_ACQUISITION_PID=$!
echo "画像取得プログラム PID: $IMAGE_ACQUISITION_PID"

# 少し待機（画像取得が安定するまで）
sleep 3

# 2. 画像処理プログラムを起動
echo ""
echo "=== 2. 画像処理プログラムを起動中... ==="
echo "画像処理プログラム (IAnaS.py) を起動..."
python IAnaS.py &
IMAGE_ANALYSIS_PID=$!
echo "画像処理プログラム PID: $IMAGE_ANALYSIS_PID"

# 少し待機（画像処理が安定するまで）
sleep 3

# 仮想環境を非アクティブ化
deactivate

# 3. メインプログラムを起動
echo ""
echo "=== 3. メインプログラムを起動中... ==="
cd "$WORKSPACE_DIR"

echo "メインプログラム (make start) を起動..."
make start &
MAIN_PID=$!
echo "メインプログラム PID: $MAIN_PID"

echo ""
echo "=== 全プログラムの起動完了 ==="
echo "画像取得プログラム PID: $IMAGE_ACQUISITION_PID"
echo "画像処理プログラム PID: $IMAGE_ANALYSIS_PID"
echo "メインプログラム PID: $MAIN_PID"
echo ""
echo "プログラムを停止するには以下のコマンドを実行してください:"
echo "kill $IMAGE_ACQUISITION_PID $IMAGE_ANALYSIS_PID $MAIN_PID"
echo ""
echo "または、Ctrl+C を押してこのスクリプトを終了してください。"

# シグナルハンドラーを設定（Ctrl+Cで全プロセスを終了）
cleanup() {
    echo ""
    echo "=== プログラムを終了中... ==="
    echo "画像取得プログラムを終了..."
    kill $IMAGE_ACQUISITION_PID 2>/dev/null || true
    echo "画像処理プログラムを終了..."
    kill $IMAGE_ANALYSIS_PID 2>/dev/null || true
    echo "メインプログラムを終了..."
    kill $MAIN_PID 2>/dev/null || true
    echo "全プログラムが終了しました。"
    exit 0
}

trap cleanup SIGINT SIGTERM

# メインプロセスの終了を待機
wait $MAIN_PID
