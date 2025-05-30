#include "OnRightEdgeCloser.h"

ICloserGenerator onRightEdgeCloserGenerator() {
    return [](Device*& device) {
        return new OnRightEdgeCloser(device);
    };  
}

OnRightEdgeCloser::OnRightEdgeCloser(Device*& device)
: ICloser()
, mDevice(device)
{
}

OnRightEdgeCloser::~OnRightEdgeCloser()
{
}

void OnRightEdgeCloser::init()
{
}

bool OnRightEdgeCloser::isClosed() {
    cv::Mat image;

    /**
     * ガウシアンフィルタを適用
     * ノイズを軽減するために5x5のカーネルサイズでぼかしを適用
     */
    cv::GaussianBlur(report.image, image, cv::Size(5, 5), 0);

    /**
     * HSV色空間に変換
     * BGR色空間からHSV色空間に変換して、明度(V)による処理を行いやすくする
     */
    cv::cvtColor(image, image, cv::COLOR_BGR2HSV);
    
    /**
     * V値による二値化
     * 黒い床の模様を抽出するため、V=0~100の暗い部分を白（255）として抽出
     * inRangeは指定範囲内を255、範囲外を0にする
     * 黒い模様は色相に関係なく暗いので、H,Sは全範囲、Vのみ0~100に制限
     */
    cv::Mat binaryImage;
    cv::inRange(image, cv::Scalar(0, 100, 0), cv::Scalar(179, 255, 100), binaryImage);

    /**
     * 画像の右下4分の1の領域を抽出
     */
    int height = binaryImage.rows;
    int width = binaryImage.cols;
    
    // 右下4分の1の領域を定義 (x: width*3/5 ~ width, y: height*9/10 ~ height)
    cv::Rect bottomRightROI((width * 3 / 5), (height * 9 / 10), (width * 2 / 5), (height * 1 / 10));
    cv::Mat bottomRightRegion = binaryImage(bottomRightROI);
    
    /**
     * 右下領域に黒い部分（0の画素）があるかチェック
     * countNonZero()は0以外の画素数を返すので、
     * 全画素数と比較して黒い画素があるかを判定
     */
    int whitePixels = cv::countNonZero(bottomRightRegion);
    
    // 白い画素がない（すべて黒い）場合にtrueを返す
    bool hasNoBlackPixels = (whitePixels == 0);

    /**
     * XXX　床色の判定を加えることで、判定が堅牢になる可能性を検討する
     */
    
    return hasNoBlackPixels;
}