



//#include "opencv.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(void)
{

    float dataA[] = {-1, -2, -3, 4, 5, 6};
    float dataB[] = {1, 2, 3, 4, 5, 6};
    Mat A(2, 3, CV_32F, dataA);
    Mat B(2, 3, CV_32F, dataB);
    cout << "A = " << A << endl;
    cout << "B = " << B << endl;

    double aNormL2 = norm(A); //normType = NORM_L2
    double aNormL1 = norm(A, NORM_L1);
    double aNormInf = norm(A, NORM_INF);

    cout << "aNormL1 = " << aNormL1 << endl;
    cout << "aNormL2 = " << aNormL2 << endl;
    cout << "aNormInf = " << aNormInf << endl;

    double bNormL2 = norm(B); // normType = NORM_L2
    double bNormL1 = norm(B, NORM_L1);
    double bNormInf = norm(B, NORM_INF);
    cout << "bNormL1 = " << bNormL1 << endl;
    cout << "bNormL2 = " << bNormL2 << endl;
    cout << "bNormInf = " << bNormInf << endl;

    double abNormL2 = norm(A,B); // normType = NORM_L2
    double abNormL1 = norm(A, B, NORM_L1);
    double abNormInf = norm(A, B, NORM_INF);
    cout << "abNormL1 = " << abNormL1 << endl;
    cout << "abNormL2 = " << abNormL2 << endl;
    cout << "abNormInf = " << abNormInf << endl;

    double rNormL2 = norm(A, B, NORM_L1 + NORM_RELATIVE); // normType = NORM_L2
    double rNormL1 = norm(A, B, NORM_L2 + NORM_RELATIVE);
    double rNormInf = norm(A, B, NORM_INF + NORM_RELATIVE);
    cout << "rNormL1 = " << rNormL1 << endl;
    cout << "rNormL2 = " << rNormL2 << endl;
    cout << "rNormInf = " << rNormInf << endl;

    Mat dst1;
    normalize(A,dst1);//NORM_L2
    cout << "dst1 = " << dst1 << endl;
    double dstNorm1 = norm(dst1);
    cout << "dstNorm1 = " << dstNorm1 << endl;

    Mat dst2;
    normalize(A,dst2, 1.0, 2.0, NORM_MINMAX);//NORM_L2
    cout << "dst2 = " << dst2 << endl;

    return 0;
}
