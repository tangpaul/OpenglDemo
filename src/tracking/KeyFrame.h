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

	std::unordered_map<unsigned int, FeaturePoint> pointsMap;	// id -- undisort point	//�����һ��֮��ĵ�

	Eigen::Quaterniond qPTRRelative;	//��Ԫ����̬�������һ�ؼ�֡��
	Eigen::Quaterniond qPTRAbsolute;	//��Ԫ����̬ (������̬)

	Eigen::Quaterniond qIMURelative;	//IMU�����̬
	Eigen::Quaterniond qIMUAbsolute;	//IMU������̬
};

