#pragma once

#include "FeaturePoint.h"
#include "XICamera.h"

#include <vector>
#include <unordered_map>
#include <opencv2/opencv.hpp>

namespace COMMON
{
	extern cv::Mat binframe;
	extern unsigned long long FrameCount;
};

class Frame
{
public:
	Frame();
	Frame(cv::Mat& img);
	~Frame();

	inline bool Empty() { return isEmpty; }

	std::vector<cv::Point2d> points;							//points
	std::vector< FeaturePoint> features;						//ÌØÕ÷µã
	std::unordered_map<unsigned int, FeaturePoint> pointsMap;	// id -- point

	unsigned long long frameId = 0;

private:
	void detectPoints(cv::Mat& img);
	void constructFeatures();

private:
	bool isEmpty = true;
};
