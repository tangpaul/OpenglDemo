#include "FrameMatcher.h"
#include <iostream>
#include "Utilities.h"

FrameMatcher::FrameMatcher()
{

}

FrameMatcher::~FrameMatcher()
{

}

void FrameMatcher::InitFrame(Frame& firstFrame)
{
	for (auto& pt : firstFrame.features)
	{
		firstFrame.pointsMap.insert(std::make_pair(++featureId, pt));
	}
}

bool FrameMatcher::MatchFrames(Frame& lastFrame, Frame& curFrame)	//复杂度O(n^2) 低效匹配
{
	// Utility::Timer timer("MatchFrames");
	int matchedNum = 0;
	for (auto& curPt : curFrame.features)
	{
		bool matched = false;	//点能找到对应匹配
		for (auto& lastMap : lastFrame.pointsMap)
		{
			if (pointDistanceLeastThan(curPt.pt, lastMap.second.pt, 40))
			{
				matchedNum++;
				matched = true;
				curFrame.pointsMap.insert(std::make_pair(lastMap.first, curPt));
				break;
			}
		}
		if (!matched)
		{
			curFrame.pointsMap.insert(std::make_pair(++featureId, curPt));
			//std::cout << "new point detect, id:" << featureId << "\n";
		}
	}

	if(matchedNum < 10)
		std::cout << "matched point is too few.\n";

	return (matchedNum > 0);
}

bool FrameMatcher::pointDistanceLeastThan(cv::Point2d& point1, cv::Point2d& point2, double distance)
{
	return ((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y)) < distance * distance;
}
