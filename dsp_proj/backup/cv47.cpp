



//#include "opencv.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main(void)
{
    //VideoCapture capture("output.avi");
    VideoCapture capture("super_hard.mp4");

    if(!capture.isOpened())
    {
        cout << " Can not open capture!!!" << endl;
        return 0;

    }
    int ex = (int)(capture.get(CAP_PROP_FOURCC));
    char fourcc[] = {ex&0XFF, (ex&0XFF00) >> 8, (ex&0XFF0000) >> 16, (ex&0XFF000000)>>24, 0};
    cout << "fourcc = " << fourcc << endl;

    Size size = Size((int) capture.get(CAP_PROP_FRAME_WIDTH), (int)capture.get(CAP_PROP_FRAME_HEIGHT));
    cout <<"Size = " << size << endl;

    int fps = (int)(capture.get(CAP_PROP_FPS));
    cout << "fps = " << fps << endl;

    int delay = 1000/fps;
    int frameNum = -1;
    Mat frame, grayImage, edgeImage;

    namedWindow("frame", WINDOW_AUTOSIZE);
    namedWindow("edgeImage", WINDOW_AUTOSIZE);

    for(;;)
    {
        capture >> frame;

        if(frame.empty())
            break;

        //cvtColor(frame, grayImage, COLOR_BGR2GRAY);
        //Canny(grayImage, edgeImage, 100, 200, 3);

        imshow("frame", frame);
        //imshow("edgeImage", edgeImage);
        waitKey(delay);
        int ckey = waitKey(delay);
        if(ckey == 27)
            break;
    }

    return 0;

}
