#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#define INF INT_MAX
using namespace std;
using namespace cv;
const double PI = 3.1415926;

struct cluster {
    int l;
    int a;
    int b;
    int x;
    int y;
};

double get_dis(Mat src, struct cluster cluster, int x, int y, int s, int m) {
    int dis_l = cluster.l - src.at<Vec3b>(x, y)[0];
    int dis_a = cluster.a - src.at<Vec3b>(x, y)[1];
    int dis_b = cluster.b - src.at<Vec3b>(x, y)[2];
    int dis_x = cluster.x - x;
    int dis_y = cluster.y - y;

    double d_c = sqrt(pow(dis_l, 2) + pow(dis_a, 2) + pow(dis_b, 2));
    double d_s = sqrt(pow(dis_x, 2) + pow(dis_y, 2));
    return sqrt(pow(d_c, 2) + pow((double)d_s / sqrt(s), 2) * pow(m,2));
}

/* SLIC超像素算法实现 */
void mySLIC(Mat &src, Mat &dst, int k, int m_importance, int iteration_times) {
    Mat src_lab;
    // 转化为lab彩色空间
    cvtColor(src, src_lab, COLOR_BGR2Lab);
    int rows = src.rows;
    int cols = src.cols;
    printf("图像大小：w*h=%d*%d\n", cols, rows);

    CV_Assert(rows == cols);

    int total_n = rows * rows; // 像素总数
    int total_k = k * k;       // 超像素总数
    int s = rows / k;          // 超像素之间的距离

    // 初始化
    struct cluster clusters[k][k];
    pair<int, int> label[rows][cols];
    double dis[rows][cols];
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            clusters[i][j].x = s / 2 + i * s;
            clusters[i][j].y = s / 2 + j * s;
            clusters[i][j].l =
                src_lab.at<Vec3b>(s / 2 + i * s, s / 2 + j * s)[0];
            clusters[i][j].a =
                src_lab.at<Vec3b>(s / 2 + i * s, s / 2 + j * s)[1];
            clusters[i][j].b =
                src_lab.at<Vec3b>(s / 2 + i * s, s / 2 + j * s)[2];
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dis[i][j] = INF;
        }
    }

    for (int q = 0; q < iteration_times; q++) {
        printf("迭代次数：%d\n", q);
        // 聚类
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                int center_x = clusters[i][j].x;
                int center_y = clusters[i][j].y;
                for (int m = -2 * s; m < 2 * s; m++) {
                    for (int n = -2 * s; n < 2 * s; n++) {
                        int x = center_x + m;
                        int y = center_y + n;
                        if (x < 0 || x > rows || y < 0 || y > cols)
                            continue;
                        int d = get_dis(src_lab, clusters[i][j], x, y, s, m_importance);
                        if (dis[x][y] > d) {
                            dis[x][y] = d;
                            label[x][y].first = i;
                            label[x][y].second = j;
                        }
                    }
                }
            }
        }
        int sum[k][k];
        int sum_x[k][k];
        int sum_y[k][k];
        int sum_l[k][k];
        int sum_a[k][k];
        int sum_b[k][k];
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                sum[i][j] = 0;
                sum_x[i][j] = 0;
                sum_y[i][j] = 0;
                sum_l[i][j] = 0;
                sum_a[i][j] = 0;
                sum_b[i][j] = 0;
            }
        }

        // 更新超像素点
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int x = label[i][j].first;
                int y = label[i][j].second;
                sum[x][y] += 1;
                sum_x[x][y] += i;
                sum_y[x][y] += j;
                sum_l[x][y] += src_lab.at<Vec3b>(i, j)[0];
                sum_a[x][y] += src_lab.at<Vec3b>(i, j)[1];
                sum_b[x][y] += src_lab.at<Vec3b>(i, j)[2];
            }
        }

        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                if (sum[i][j] == 0)
                    continue;
                clusters[i][j].x = sum_x[i][j] / sum[i][j];
                clusters[i][j].y = sum_y[i][j] / sum[i][j];
                clusters[i][j].l = sum_l[i][j] / sum[i][j];
                clusters[i][j].a = sum_a[i][j] / sum[i][j];
                clusters[i][j].b = sum_b[i][j] / sum[i][j];
            }
        }
    }
    Mat tmp = Mat::zeros(rows, cols, src_lab.type());

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int x = label[i][j].first;
            int y = label[i][j].second;
            tmp.at<Vec3b>(i, j)[0] =
                src_lab.at<Vec3b>(clusters[x][y].x, clusters[x][y].y)[0];
            tmp.at<Vec3b>(i, j)[1] =
                src_lab.at<Vec3b>(clusters[x][y].x, clusters[x][y].y)[1];
            tmp.at<Vec3b>(i, j)[2] =
                src_lab.at<Vec3b>(clusters[x][y].x, clusters[x][y].y)[2];
        }
    }

    cvtColor(tmp, dst, COLOR_Lab2BGR);
}

int main(int argc, char **argv) {
    Mat src = imread("../imgs/lenna.jpg", 1);
    // 晶格化实现
    Mat dst_my;
    // k含义是大小大致相等的超像素的个数
    int k = 64;
    int m_importance = 30;
    int iteration_times = 10;
    mySLIC(src, dst_my, k, m_importance, iteration_times);
    imwrite("../imgs/out_lenna_my.jpg", dst_my);
    return 0;
}
