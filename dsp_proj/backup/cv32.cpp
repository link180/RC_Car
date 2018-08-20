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
    Mat srcImage1 = imread("Gna.jpg", -1);

    if(srcImage1.empty())
        return -1;

    cout << "srcImage1.type() " << srcImage1.type() << endl;
    cout << "srcImage1.depth()" << srcImage1.depth() << endl;
    cout << "srcImage1.channels() " << srcImage1.channels() << endl;

    Mat srcImage2 = imread("iu.jpeg" , IMREAD_COLOR);

    if(srcImage2.empty())
        return -1;

    cout << "srcImage2.type() = " << srcImage2.type() << endl;
    cout << "srcImage2.depth() = " << srcImage2.depth() << endl;
    cout << "srcImage2.channels() = " << srcImage2.channels() << endl;

    Mat srcImage3 = imread("lhs1.jpg",IMREAD_GRAYSCALE);

    if(srcImage3.empty())
        return -1;

    cout << "srcImage3.type() = " << srcImage3.type() << endl;
    cout << "srcImage3.depth() = " << srcImage3.depth() << endl;
    cout << "srcImage3.channels() = " << srcImage3.channels() << endl << endl;

    imwrite("Gna.jpg", srcImage1);

    vector<int> params;
    params.push_back(IMWRITE_PNG_COMPRESSION);
    params.push_back(9);
   // imwrite("lhs.jpg", srcImage3, params);

    imshow("Gna.jpg", srcImage1);
    imshow("iu.jpeg", srcImage2);
    imshow("lhs.jpg", srcImage3);

    waitKey();

    return 0;
}
