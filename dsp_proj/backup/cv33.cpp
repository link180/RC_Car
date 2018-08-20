/*
 * cv27.cpp
 *
 *  Created on: Jun 27, 2018
 *      Author: howi
 */




//#include "opencv.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;



int main(void)
{
    Mat srcImage = imread("Gna.jpg", IMREAD_GRAYSCALE);

    if(srcImage.empty())
        return -1;

    Rect rtROI(120, 150, 200, 200);
    Mat roi = srcImage(rtROI);
    rectangle(srcImage, rtROI, Scalar::all(0));
    imshow("srcImage",srcImage);

    Mat dstImage = repeat(roi, 2, 2);
    imshow("dstImage", dstImage);


    waitKey();



    return 0;
}
