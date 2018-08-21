#include <opencv2/opencv.hpp>
#include "LaneDetector.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <iostream>

#include <atomic>

std::atomic<bool> end = false;

struct Frame
{
	cv::Mat frame;
	unsigned long seq_no;
};

class SafeQueue
{
public:
	SafeQueue() : counter_(0) { }

	void push(cv::Mat frame)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		frames_.emplace(frame);
	}

	bool pop(cv::Mat& frame)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		if(!frames_.empty())
		{
		    frame = frames_.front();
		    frames_.pop();
		    return true;
		}
		
		return false;
	}

private:
	std::queue<cv::Mat> frames_;
	unsigned long counter_;
	std::mutex mtx_;
}

SafeQueue queueFrames;
SafeQueue queueOutput;

cv::Mat DrawRoadModel(const vision::RoadModel& roadModel, const cv::Mat& inputFrame)
{
    cv::Mat color{ inputFrame };
    const cv::Vec3b a = {0, 255, 0};
    const cv::Vec3b b = {0, 255, 255};

    return color;
}

void processFrame(void)
{
    vision::LaneDetector laneDetector;
    cv::Mat frame;

    unsigned long frame_no = 0;
    unsigned long avg = 0;

    while(end == false)
    {
        if(queueFrames.pop(frame) == false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto beg = std::chrono::high_resolution_clock::now();

        auto local = frame.clone();
        auto roadModel = laneDetector.DetectLane(local);
        auto output = DrawRoadModel(roadModel, local);
        queueOutput.push(output);

        ++frame_no;

        auto diff = std::chrono::duration_cast<std::chrono::milliseconds > (std::chrono::high_resolution_clock::now() - beg).count();

        avg += diff;
        std::cout << "frame:\t" << frame_no << "\ttime:\t" << diff << "\tms\t" << avg / frame_no << "\tms\t" << 1000 / diff << std::endl;
    }
}

void grabFrames(void)
{
    cv::VideoCapture videoCapture("basic_curve.mp4");

    if(videoCapture.isOpened() == false)
        end = true;

    cv::Mat frame;

    while(end == false)
    {
        videoCapture >> frame;

        if(frame.empty())
            end = true;
        else
            queueFrames.push(frame);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    end = true;
}

void displayResult(void)
{
    cv::namedWindow("LaneDetector");

    cv::Mat frame;

    while(end == false)
    {
        if(queueOutput.pop(frame) == false)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1))
            continue;
        }

        cv::imshow("LaneDetector", frame);

        if(cv::waitKey(3) == 27)
            break;
    }

    end = true;
    cv::destroyAllWindows();
}

int main(int argc, char **argv)
{
    std::vector<std::thread> threads;

    threads.emplace_back(displayResult);
    threads.emplace_back(grabFrames);
    threads.emplace_back(processFrame);

    for(auto& t : threads)
        t.join();

    return 0;
}
