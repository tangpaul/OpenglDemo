#pragma once

#include "Frame.h"
#include "XICamera.h"

class FrameMatcher
{
public:
	FrameMatcher();
	~FrameMatcher();

	void InitFrame(Frame& firstFrame);
	bool MatchFrames(Frame& lastFrame, Frame& curFrame);

private:
	bool pointDistanceLeastThan(cv::Point2d& point1, cv::Point2d& point2, double distance);

private:
	unsigned int featureId	= 0;
};

