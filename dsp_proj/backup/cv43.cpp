



//#include "opencv.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(void)
{
    Mat srcImage = imread("Gna.jpg");
    if(srcImage.empty())
        return -1;

    imshow("srcImage", srcImage);

    Mat grayImage;
    cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
    imshow("grayImage", grayImage);

    Mat hsvImage;
    cvtColor(srcImage, hsvImage, COLOR_BGR2HSV);
    imshow("hsvImage", hsvImage);

    Mat yCrCbImage;
    cvtColor(srcImage, yCrCbImage, COLOR_BGR2YCrCb);
    imshow("yCrCvCbImage", yCrCbImage);

    Mat luvImage;
    cvtColor(srcImage, luvImage, COLOR_BGR2Luv);
    imshow("luvImage", luvImage);
    waitKey();

    waitKey();

    return 0;
}
