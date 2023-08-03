#pragma once

#include "KeyFrame.h"
#include "Frame.h"
#include "FrameMatcher.h"
#include "XICamera.h"

#include <set>
#include <list>
#include <opencv2/opencv.hpp>

#include "ITrackerListener.h"

namespace SLAM{ 

const int WINDOW_SIZE = 30;

class Tracker
{
public:
	enum STATE	//追踪状态
	{
		NOTINITILIZE,	//未初始化
		INITILIZED		//已初始化
	};

public:
	Tracker();
	~Tracker();

	void GrabImage(cv::Mat& image, Eigen::Quaterniond qimu);
	void InitKeyFrame(Frame& initframe);
	void SoloveRotation(KeyFrame& keyframe, Frame& curFrame);
	void ImportKeyFrame(KeyFrame& lastKeyframe, Frame& curFrame, Eigen::Quaterniond& qR);
	void DrawFeatureIds(cv::Mat& rgbFrame);
	void RegisterListener(ITrackerListener* host);

	Eigen::Quaterniond GetCurrentQuat();

public:
	std::vector<KeyFrame> keyframes;
	STATE state;

private:
	Eigen::Quaterniond caculateRotation(std::vector<cv::Point2f>& leftPoints, std::vector<cv::Point2f>& rightPoints);
	void calibrationImu2Cam();
	void vertifyCalibration();

	void decomposeE(cv::Mat E,
		cv::Mat_<double>& R1, cv::Mat_<double>& R2,
		cv::Mat_<double>& t1, cv::Mat_<double>& t2);
	double testTriangulation(const std::vector<cv::Point2f>& l,
		const std::vector<cv::Point2f>& r,
		cv::Mat_<double> R, cv::Mat_<double> t);

private:
	Frame mLastFrame;	//上一帧
	Frame mCurFrame;	//这一帧

	FrameMatcher matcher;	//帧匹配

	const Eigen::Quaterniond StdQuat = Eigen::Quaterniond(1, 0, 0, 0);
	Eigen::Quaterniond mQCurIMU = Eigen::Quaterniond(1, 0, 0, 0);

	ITrackerListener*	mListener = nullptr;
	Eigen::Quaterniond mQIMU2Cam = Eigen::Quaterniond(1, 0, 0, 0);
};

}