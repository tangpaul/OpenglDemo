#include "Frame.h"
#include "Utilities.h"

cv::Mat COMMON::binframe = cv::Mat();
unsigned long long COMMON::FrameCount = 0;

Frame::Frame(cv::Mat& img)
{
	//提取特征点
	{
		// Utility::Timer timer("detectPoints");
		detectPoints(img);
	}

	if(points.size() > 0)
		isEmpty = false;
	else
		isEmpty = true;

	//矫正畸变 && 归一化点
	{
		// Utility::Timer timer("constructFeatures");
		if(!isEmpty) constructFeatures();
	}

	frameId = ++COMMON::FrameCount;
}

Frame::Frame()
{
	isEmpty = true;
}

Frame::~Frame()
{
}

void Frame::detectPoints(cv::Mat& frame)
{
	using namespace cv;
	{
		// Utility::Timer timer("detectPoints threshold");
		threshold(frame, COMMON::binframe, 80, 255, THRESH_BINARY);
	}
	std::vector < std::vector<cv::Point> > contours;
	{
		// Utility::Timer timer("detectPoints findContours");
		findContours(COMMON::binframe, contours, RETR_LIST, CHAIN_APPROX_NONE);
	}

	for(const auto& contour : contours){
		double area = cv::contourArea(contour);
		if(area < 15 || area > 1000) continue;

		cv::Moments moment = cv::moments(contour);
		cv::Point2d midPoint = cv::Point2d(moment.m10 / moment.m00, moment.m01 / moment.m00);
		points.push_back(midPoint);
	}
}

void Frame::constructFeatures()
{
	//求解归一化点
	std::vector<cv::Point2d> normalPoints;
	cv::undistortPoints(points, normalPoints, TKParams::cameraMatrix, TKParams::distCoeffs);

	for(int i = 0; i < points.size(); i++)	{
		features.push_back(FeaturePoint(points[i], normalPoints[i]));
	}
}
