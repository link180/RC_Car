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
    float dataA[] = {1, 2, 3, 4, 5, 6, 7 ,8};

    Mat A(2, 4, CV_32F, dataA);
    cout << "A = " << endl << A << endl;

    Mat dst1 = A.clone();
    randShuffle(dst1);
    cout << "dst1 = " << endl << dst1 << endl;

    Mat dst2 = A.clone();
    randShuffle(dst2, 10);
    cout << "dst2 = " << endl << dst2 << endl;
    return 0;

    return 0;
}
