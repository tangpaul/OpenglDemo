#include "Tracker.h"
#include "Utilities.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace SLAM{

Tracker::Tracker()
{
	state = NOTINITILIZE;
}

Tracker::~Tracker()
{

}

void Tracker::GrabImage(cv::Mat& image, Eigen::Quaterniond qimu)
{
	mQCurIMU = qimu;

	if(state == NOTINITILIZE)		//未初始化
	{
		Frame firstFrmae(image);
		if(firstFrmae.points.size() > 17)		{	//初始化第一帧，要求识别到的点大于17个
			matcher.InitFrame(firstFrmae);
			this->InitKeyFrame(firstFrmae);
			mLastFrame = firstFrmae;
			state = INITILIZED;
			std::cout << "initilzed!\n";
		}
	}
	else if(state == INITILIZED)	//初始化
	{
		mCurFrame = Frame(image);							//构造帧，用于识别特征点
		if(mCurFrame.points.size() < 10)		{
			std::cout << "detected points is too few.\n";
		}

		if(!matcher.MatchFrames(mLastFrame, mCurFrame)){	//匹配两帧之间的特征点
			std::cout << "match points failed!\n";
		}

		//挑选关键帧（策略：距离上一关键帧角度变化足够大，或者，距离上一关键帧帧数足够远(防止因噪声导致时间长了追踪失败)）
		{
			// Utility::Timer timer("SoloveRotation");
			SoloveRotation(keyframes[keyframes.size() - 1], mCurFrame);
		}

		mLastFrame = mCurFrame;
	}
}

void Tracker::InitKeyFrame(Frame& initframe)
{
	KeyFrame keyframe;
	keyframe.frameId = initframe.frameId;

	//畸变矫正 && 归一化点
	keyframe.pointsMap = initframe.pointsMap;

	//初始姿态
	keyframe.qPTRAbsolute = Eigen::Quaterniond(1, 0, 0, 0);
	keyframe.qPTRRelative = Eigen::Quaterniond(1, 0, 0, 0);

	//imu
	keyframe.qIMUAbsolute = mQCurIMU;
	keyframe.qIMURelative = Eigen::Quaterniond(1, 0, 0, 0);

	keyframes.push_back(keyframe);
}

void Tracker::SoloveRotation(KeyFrame& keyframe, Frame& curFrame)
{
	std::vector<cv::Point2f> ll, rr;
	for (auto& pt : curFrame.pointsMap)
	{
		auto it = keyframe.pointsMap.find(pt.first);
		if(it != keyframe.pointsMap.end()){
			ll.push_back(pt.second.ptNormal);
			rr.push_back(it->second.ptNormal);
		}
	}
	//std::cout << "find " << ll.size() << " matches.\n";

	Eigen::Quaterniond qRelativeR = caculateRotation(ll, rr);
	double angelR = 180.0 / M_PI * qRelativeR.angularDistance(StdQuat);

	// unsigned long long frameCount = curFrame.frameId - keyframe.frameId;

	if(angelR >= 3.0){	//角度大于3度，帧差300帧
		//导入关键帧
		ImportKeyFrame(keyframe, curFrame, qRelativeR);
		std::cout << "angel: " << angelR << "\n";
		
		//求解 qImuToCam
		calibrationImu2Cam();

		//验证
		vertifyCalibration();
	}
}

void Tracker::ImportKeyFrame(KeyFrame& lastKeyframe, Frame& curFrame, Eigen::Quaterniond& qR)
{
	KeyFrame curKeyFrame;
	curKeyFrame.frameId = curFrame.frameId;
	curKeyFrame.pointsMap = curFrame.pointsMap;
	curKeyFrame.qPTRRelative = qR.inverse();
	curKeyFrame.qPTRAbsolute = qR.inverse() * lastKeyframe.qPTRAbsolute;

	Eigen::Vector3d eulerAngle = curKeyFrame.qPTRAbsolute.matrix().eulerAngles(0, 1, 2) * 180.0 / M_PI;
	std::cout << "import 1 key frame, PTR:" << eulerAngle.x() << " " << eulerAngle.y() << " " << eulerAngle.z() << "\n";

	curKeyFrame.qIMUAbsolute = mQCurIMU;
	curKeyFrame.qIMURelative = lastKeyframe.qIMUAbsolute.inverse() * curKeyFrame.qIMUAbsolute ;

	keyframes.push_back(curKeyFrame);
	if(mListener != nullptr) mListener->QuatUpdate(curKeyFrame.qPTRAbsolute, curKeyFrame.qIMUAbsolute , curKeyFrame.qIMURelative);
}

void Tracker::DrawFeatureIds(cv::Mat& rgbFrame)
{
	for (auto& pt : mLastFrame.pointsMap)
	{
		cv::putText(rgbFrame, std::to_string(pt.first), pt.second.pt, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
	}
}

void Tracker::RegisterListener(ITrackerListener* host){
	mListener = host;
}

Eigen::Quaterniond Tracker::GetCurrentQuat()
{
	if(keyframes.size() > 0){
		return keyframes[keyframes.size() - 1].qPTRAbsolute;
	}else{
		return Eigen::Quaterniond(1,0,0,0);
	}
}

Eigen::Quaterniond Tracker::caculateRotation(std::vector<cv::Point2f>& leftPoints, std::vector<cv::Point2f>& rightPoints)
{
	using namespace cv;
	//求解单应矩阵
	Mat E;
	E = findFundamentalMat(leftPoints, rightPoints);
 
	//求解旋转
	cv::Mat_<double> R1, R2, t1, t2;
	decomposeE(E, R1, R2, t1, t2);

	double detR = determinant(R1);
	if(detR + 1.0 < 1e-09){
		E = -E;
		decomposeE(E, R1, R2, t1, t2);
		//cout << "Reverse E." << endl;
		//cout << "E is : " << endl << E << endl;
	}

	//测试4种组合的三角测量深度，决定是哪一个组合
	double ratio1 = max(testTriangulation(leftPoints, rightPoints, R1, t1),
						testTriangulation(leftPoints, rightPoints, R1, t2));
	double ratio2 = max(testTriangulation(leftPoints, rightPoints, R2, t1),
						testTriangulation(leftPoints, rightPoints, R2, t2));

	cv::Mat_<double> ans_R_cv = ratio1 > ratio2 ? R1 : R2;
	Eigen::Matrix3d ans_R_eigen;
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			ans_R_eigen(j, i) = ans_R_cv(i, j);

	return Eigen::Quaterniond(ans_R_eigen);
}

//求解相机到Imu的旋转
void Tracker::calibrationImu2Cam()	
{
	if(keyframes.size() < WINDOW_SIZE) return;

	Eigen::MatrixXd A((WINDOW_SIZE) * 4, 4);
	A.setZero();
	int sum_ok = 0;
	for (size_t i = keyframes.size()-WINDOW_SIZE; i < keyframes.size(); i++)
	{
		// Eigen::Quaterniond r1(keyframes[i].qPTRRelative);
		// Eigen::Quaterniond r2(keyframes[i].qIMURelative);

		// double angular_distance = 180 / M_PI * r1.angularDistance(r2);
		// double huber = angular_distance > 5.0 ? 5.0 / angular_distance : 1.0;
		// printf("angular distance: %f huber: %f\n", angular_distance, huber);

		Eigen::Matrix4d L, R;

		double w = Eigen::Quaterniond(keyframes[i].qPTRRelative).w();
        Eigen::Vector3d q = Eigen::Quaterniond(keyframes[i].qPTRRelative).vec();
        L.block<3, 3>(0, 0) = w * Eigen::Matrix3d::Identity() + Utility::skewSymmetric(q);
        L.block<3, 1>(0, 3) = q;
        L.block<1, 3>(3, 0) = -q.transpose();
        L(3, 3) = w;

		Eigen::Quaterniond R_ij(keyframes[i].qIMURelative);
        w = R_ij.w();
        q = R_ij.vec();
        R.block<3, 3>(0, 0) = w * Eigen::Matrix3d::Identity() - Utility::skewSymmetric(q);
        R.block<3, 1>(0, 3) = q;
        R.block<1, 3>(3, 0) = -q.transpose();
        R(3, 3) = w;

		A.block<4, 4>(sum_ok++ * 4, 0) = (L - R);
	}
	
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix<double, 4, 1> x = svd.matrixV().col(3);
    Eigen::Quaterniond estimated_R(x(3,0), x(0,0), x(1,0), x(2,0));
	mQIMU2Cam = estimated_R;

	// Eigen::Vector3d ric_cov;	//误差协方差，看起来是越大越好？
    // ric_cov = svd.singularValues().tail<3>();
	// double error = ric_cov(1);

	// mQIMU2Cam = Eigen::Quaterniond(0.0006148877061634255, -0.0016745996562508494, -0.7068177268442827, 0.7073934680556367);
	// if(mListener != nullptr) mListener->CalibUpdate(mQIMU2Cam, error);
}

void Tracker::vertifyCalibration(){
	if(keyframes.size() < WINDOW_SIZE) return;

	double angular_avg = 0.0, angular_min = 100.0, angular_max = DBL_MIN;
	for (size_t i = keyframes.size()-WINDOW_SIZE; i < keyframes.size(); i++){
		KeyFrame& lastKeyFrame = keyframes[i];
		Eigen::Quaterniond imuCalib = mQIMU2Cam * lastKeyFrame.qIMURelative * mQIMU2Cam.inverse();
		double angular_distance = 180.0 / M_PI * imuCalib.angularDistance(lastKeyFrame.qPTRRelative);
		angular_avg += angular_distance;
		if(angular_distance < angular_min ) angular_min = angular_distance;
		if(angular_distance > angular_max ) angular_max = angular_distance;
		printf("error:%f\n", angular_distance);
	}
	angular_avg /= WINDOW_SIZE;
	
	printf("qI2C w:%.20f, x:%.20f, y:%.20f, z:%.20f ;error:%f, max:%f, min:%f\n",
			 mQIMU2Cam.w(), mQIMU2Cam.x(), mQIMU2Cam.y(), mQIMU2Cam.z(), angular_avg, angular_max, angular_min);

	if(mListener != nullptr) mListener->CalibUpdate(mQIMU2Cam, angular_avg, angular_max, angular_min);
}


void Tracker::decomposeE(cv::Mat E, cv::Mat_<double>& R1, cv::Mat_<double>& R2, cv::Mat_<double>& t1, cv::Mat_<double>& t2)
{
	cv::SVD svd(E, cv::SVD::MODIFY_A);
	cv::Matx33d W(0, -1, 0,
		1, 0, 0,
		0, 0, 1);
	cv::Matx33d Wt(0, 1, 0,
		-1, 0, 0,
		0, 0, 1);
	R1 = svd.u * cv::Mat(W) * svd.vt;
	R2 = svd.u * cv::Mat(Wt) * svd.vt;
	t1 = svd.u.col(2);
	t2 = -svd.u.col(2);
}

double Tracker::testTriangulation(const std::vector<cv::Point2f>& l, const std::vector<cv::Point2f>& r, cv::Mat_<double> R, cv::Mat_<double> t)
{
	cv::Mat pointcloud;
	cv::Matx34f P = cv::Matx34f(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0);
	cv::Matx34f P1 = cv::Matx34f(R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), R(2, 2), t(2));
	cv::triangulatePoints(P, P1, l, r, pointcloud);
	int front_count = 0;
	for(int i = 0; i < pointcloud.cols; i++){
		double normal_factor = pointcloud.col(i).at<float>(3);

		cv::Mat_<double> p_3d_l = cv::Mat(P) * (pointcloud.col(i) / normal_factor);
		cv::Mat_<double> p_3d_r = cv::Mat(P1) * (pointcloud.col(i) / normal_factor);
		if(p_3d_l(2) > 0 && p_3d_r(2) > 0)
			front_count++;
	}
	//printf("MotionEstimator: %f\n", 1.0 * front_count / pointcloud.cols);
	return 1.0 * front_count / pointcloud.cols;
}

}