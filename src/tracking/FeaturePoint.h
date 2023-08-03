#pragma once

#include <opencv2/opencv.hpp>

struct FeaturePoint
{
	FeaturePoint(cv::Point2d& point, cv::Point2d& pointnormal)
	{
		pt = point;
		ptNormal = pointnormal;
	}

	cv::Point2d pt;
	cv::Point2d ptNormal;
};