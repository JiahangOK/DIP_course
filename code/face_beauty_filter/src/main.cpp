#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace std;
using namespace cv;
const double PI = 3.1415926;

/* 生成空间模版 */
vector<vector<double>> get_space_template(double sigma, int d) {
    int core_x = d / 2;
    int core_y = d / 2;
    vector<vector<double>> space_weight;
    for (int x = 0; x < d; x++) {
        vector<double> v;
        for (int y = 0; y < d; y++) {
            double t1 = (pow(x - core_x, 2) + pow(y - core_y, 2)) /
                        (2.0 * sigma * sigma);
            double temp = exp(-t1);

            v.push_back(temp);
            printf("%.3f\t", temp);
        }
        printf("\n");
        space_weight.push_back(v);
    }
    return space_weight;
}

/* 生成颜色模版 */
vector<double> get_color_template(int channel_num, double sigma_color) {
    vector<double> color_weight;
    double sum = 0.0;
    for (int i = 0; i <= 255; i++) {
        double tmp = exp(-1.0 * (i * i) / (2.0 * sigma_color * sigma_color));
        printf("%.3f\t", tmp);
        color_weight.push_back(tmp);
    }
    printf("\n");
    return color_weight;
}

/* 自己实现的双边滤波 */
void myBilateralFilter(Mat &src, Mat &dst, int d, double sigma_color,
                       double sigma_space) {

    // 获取图片通道数
    int channel_num = src.channels();
    cout << "颜色通道数：" << channel_num << endl;

    // 保证滤波器大小是个奇数
    CV_Assert(d % 2 == 1);

    // 人脸检测
    // 加载Haar或LBP对象或人脸检测器
    CascadeClassifier faceDetector;
    std::string faceCascadeFilename =
        "../data/haarcascade_frontalface_default.xml";

    //友好错误信息提示
    try {
        faceDetector.load(faceCascadeFilename);
    } catch (cv::Exception e) {
    }
    if (faceDetector.empty()) {
        std::cerr << "脸部检测器不能加载 (";
        std::cerr << faceCascadeFilename << ")!" << std::endl;
        exit(1);
    }

    // 人脸检测只试用于灰度图像
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // 直方图均匀化(改善图像的对比度和亮度)
    Mat equalizedImg;
    equalizeHist(gray, equalizedImg);

    int flags = CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH;
    Size minFeatureSize(30, 30);
    float searchScaleFactor = 1.1f;
    int minNeighbors = 4;
    std::vector<Rect> faces;
    faceDetector.detectMultiScale(equalizedImg, faces, searchScaleFactor,
                                  minNeighbors, flags, minFeatureSize);
    cout << "共检测到：" << faces.size() << "个人脸" << endl;
    

    // 生成空间模版
    vector<vector<double>> spaceTemplate;
    cout << "生成空间模版" << endl;
    spaceTemplate = get_space_template(sigma_space, d);

    // 生成颜色模版
    vector<double> colorTemplate;
    cout << "生成颜色模版" << endl;
    colorTemplate = get_color_template(channel_num, sigma_color);

    // 进行双边滤波
    int rows = src.rows;
    int cols = src.cols;
    int start = d / 2;

    // dst = Mat::zeros(rows, cols, src.type());
    dst = src.clone();

    // 扫描图像
    for (int m = 0; m < rows; m++) {
        for (int n = 0; n < cols; n++) {
            // 忽略边缘部分
            if (m < start || m >= rows - start || n < start ||
                n >= cols - start)
                continue;
            // 忽略非人脸部分
            if (m > faces[0].y + faces[0].width || m < faces[0].y ||
                n < faces[0].x || n > faces[0].x + faces[0].width)
                continue;
            for (int c = 0; c < channel_num; c++) {
                double weight_sum = 0.0;
                double color_sum = 0.0;
                // 滤波器中计算加权和
                for (int i = -start + m; i <= start + m; i++) {
                    for (int j = -start + n; j <= start + n; j++) {
                        int value_diff = abs(src.at<cv::Vec3b>(m, n)[c] -
                                             src.at<cv::Vec3b>(i, j)[c]);
                        color_sum +=
                            colorTemplate[value_diff] *
                            spaceTemplate[i - m + start][j - n + start];
                    }
                }

                // 滤波计算
                for (int i = -start + m; i <= start + m; i++) {
                    for (int j = -start + n; j <= start + n; j++) {
                        int value_diff = abs(src.at<cv::Vec3b>(m, n)[c] -
                                             src.at<cv::Vec3b>(i, j)[c]);
                        weight_sum +=
                            src.at<cv::Vec3b>(i, j)[c] *
                            (colorTemplate[value_diff] *
                             spaceTemplate[i - m + start][j - n + start]);
                    }
                }
                dst.at<cv::Vec3b>(m, n)[c] = weight_sum / color_sum;
            }
        }
    }
    // 画出人脸框
    // cv::rectangle(dst, faces[0], cv::Scalar(255, 0, 0), 1, 8, 0);
}

int main(int argc, char **argv) {
    Mat src = imread("../imgs/lenna.jpg", 1);

    // 双边滤波 --- opencv
    // Mat dst_opencv;
    // bilateralFilter(src, dst_opencv, 15, 80, 80);
    // imwrite("../imgs/out_lenna_opencv.jpg", dst_opencv);

    // 人脸双边滤波 --- 自己实现
    Mat dst_my;
    myBilateralFilter(src, dst_my, 15, 30, 15);
    imwrite("../imgs/out_lenna_my.jpg", dst_my);
    return 0;
}
