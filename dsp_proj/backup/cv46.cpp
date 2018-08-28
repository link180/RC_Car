



//#include "opencv.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main(void)
{
    Mat srcImage = imread("Gna.jpg", IMREAD_GRAYSCALE);
    if(srcImage.empty())
        return -1;

    Mat edges;
    Canny(srcImage, edges, 50, 100);
    imshow("edges", edges);
    imshow("scrImage", srcImage);
    waitKey();

    return 0;

}
