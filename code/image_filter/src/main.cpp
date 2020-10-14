#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int main( int argc, char** argv ) {
    Mat src = imread( "/mnt/d/Projects/DIP_course/code/imgs/lenna.jpg", 1 );
    Mat dst;

    // Apply bilateral filter
    bilateralFilter(src, dst, 15, 80, 80);

    imwrite("/mnt/d/Projects/DIP_course/code/imgs/out_lenna.jpg", dst); 

    return 0;
}
