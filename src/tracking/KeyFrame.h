#pragma once

#include "FeaturePoint.h"

#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <unordered_map>

class KeyFrame
{
public:
	KeyFrame();
	~KeyFrame();

	unsigned long long frameId;

	std::unordered_map<unsigned int, FeaturePoint> pointsMap;	// id -- undisort point	//保存归一化之后的点

	Eigen::Quaterniond qPTRRelative;	//四元数姿态（相对上一关键帧）
	Eigen::Quaterniond qPTRAbsolute;	//四元数姿态 (绝对姿态)

	Eigen::Quaterniond qIMURelative;	//IMU相对姿态
	Eigen::Quaterniond qIMUAbsolute;	//IMU绝对姿态
};

