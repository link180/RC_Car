#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(void)
{
	VideoCapture inputVideo(1);

	if(!inputVideo.isOpened())
	{
		cout << "Cannot Open Input Video!!!" << endl;
		return 0;
	}

	Size size = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
	cout << "Size = " << size << endl;

	int fourcc = VideoWriter::fourcc('x', 'v', 'i', 'd');
	//int fourcc = VideoWriter::fourcc('m', 'p', '4', 'v');

	double fps = 30;
	bool isColor = true;
	VideoWriter outputVideo("output.avi", fourcc, fps, size, isColor);
	//VideoWriter outputVideo("output.mp4", fourcc, fps, size, isColor);

	if(!outputVideo.isOpened())
	{
		cout << "Cannot Open Output Video!!!" << endl;
		return 0;
	}

	if(fourcc != -1)
	{
		imshow("frame", NULL);
		waitKey(100);
	}

	int delay = 1000 / fps;
	Mat frame;

	for(;;)
	{
		inputVideo >> frame;
		if(frame.empty())
			break;

		outputVideo << frame;
		imshow("frame", frame);

		int ckey = waitKey(delay);
		if(ckey == 27)
			break;
	}

	return 0;
}
