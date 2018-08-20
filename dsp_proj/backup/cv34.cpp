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
    Mat srcImage = imread("cong.jpg", IMREAD_COLOR);

    if(srcImage.empty())
        return -1;

    Mat dstImage = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());

    int N = 32;
    int nWidth = srcImage.cols / N;
    int nHeight = srcImage.rows / N;
    int x, y;
    Rect rtROI;
    Mat roi;

    imshow("srcImage", srcImage);
    for(int i = 0; i < N + 1; i++)
        for(int j = 0; j < N + 1 ; j++)
        {
            x = j * nWidth;
            y = i * nHeight;
            rtROI = Rect(x, y, nWidth, nHeight);
            if(((j >= 5 && j <= 18) && (i >= 8) && (i <= 13)) || ((j >= 22 && j <= 33) && (i >= 10) && (i <= 13)))
            {
                roi = srcImage(rtROI);
                dstImage(rtROI) = mean(roi);
            }
            else
                dstImage = srcImage;
            if((j >= 12 && j <= 28) && (i>=16 && i <= 24))
            {
                roi = srcImage(rtROI);
                dstImage(rtROI) = mean(roi);

            }
            else
                dstImage = srcImage;
            roi = srcImage(rtROI);
            dstImage(rtROI) = mean(roi);

        }

    imshow("srcImage", srcImage);
    imshow("dstImage", dstImage);
    waitKey();

    return 0;
}
