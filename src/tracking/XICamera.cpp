#include "XICamera.h"

cv::Mat TKParams::cameraMatrix = cv::Mat(3, 3, CV_64FC1, cv::Scalar::all(0));
cv::Mat TKParams::distCoeffs = cv::Mat(1, 5, CV_64FC1, cv::Scalar::all(0));

#define HandleResult(res,place) if (res!=XI_OK) {printf("Error after %s (%d)\n",place,res);return;} else {printf("Succeed in %s \n",place);}

XIMEAVideo::XIMEAVideo()
{
    //初始化内参
    TKParams::cameraMatrix.at<double>(0, 0) = 1219.101257061767;
	TKParams::cameraMatrix.at<double>(1, 1) = 1218.998428894287;
	TKParams::cameraMatrix.at<double>(0, 2) = 1005.025497786644;
	TKParams::cameraMatrix.at<double>(1, 2) = 508.6299031759407;
	TKParams::cameraMatrix.at<double>(2, 2) = 1;

	TKParams::distCoeffs.at<double>(0, 0) = -0.2185822384257465;
	TKParams::distCoeffs.at<double>(0, 1) = 0.1148661606609262;
	TKParams::distCoeffs.at<double>(0, 2) = -0.0002048388519894511;
	TKParams::distCoeffs.at<double>(0, 3) = 1.092185478077142e-05;
	TKParams::distCoeffs.at<double>(0, 4) = -0.0301183510235968;

	//初始化图像
	memset(&mImage, 0, sizeof(mImage));
    mImage.size = sizeof(XI_IMG);

	//初始化相机
	// enabling new processing type.
    mStat = xiSetParamInt(mXiH, XI_PRM_NEW_PROCESS_CHAIN_ENABLE, XI_ON);
    HandleResult(mStat,"enabling new processing type");

    // Retrieving a handle to the camera device 
    printf("Opening first camera...\n");
    mStat = xiOpenDevice(0, &mXiH);
    HandleResult(mStat,"xiOpenDevice");

    // Setting parameters 
    printf("set camera parameters...\n");
    mStat = xiSetParamInt(mXiH, XI_PRM_AEAG, XI_OFF);
    HandleResult(mStat,"xiSetParam (automatic exposure / gain - off)");
    mStat = xiSetParamInt(mXiH, XI_PRM_EXPOSURE, 18);
    HandleResult(mStat,"xiSetParam (exposure - 16us)");
    mStat = xiSetParamFloat(mXiH, XI_PRM_GAIN, 6.0);
    HandleResult(mStat,"xiSetParam (gain - 6.0 db)");
    mStat = xiSetParamInt(mXiH, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FREE_RUN);
    HandleResult(mStat,"xiSetParam (acquisition timing mode - free run)");
    mStat = xiSetParamInt(mXiH, XI_PRM_RECENT_FRAME, XI_ON);
    HandleResult(mStat,"xiSetParam (recent frame - on)");
    mStat = xiSetParamInt(mXiH, XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);
    HandleResult(mStat,"xiSetParam (trigger source - rising edge)");
    mStat = xiSetParamInt(mXiH, XI_PRM_TRG_SELECTOR, XI_TRG_SEL_FRAME_START);
    HandleResult(mStat,"xiSetParam (trigger selector - starts the capture of one frame)");
    mStat = xiSetParamInt(mXiH, XI_PRM_BUFFER_POLICY, XI_BP_UNSAFE);
    HandleResult(mStat,"xiSetParam (buffer policy - unsafe(no copy))");

	// acquisition
    printf("Starting acquisition...\n");
    mStat = xiStartAcquisition(mXiH);
    HandleResult(mStat,"xiStartAcquisition");

    // flip
    mStat = xiSetParamInt(mXiH, XI_PRM_HORIZONTAL_FLIP, XI_OFF);
    HandleResult(mStat,"xiSetParam (Flip horizontal - on)");
    mStat = xiSetParamInt(mXiH, XI_PRM_VERTICAL_FLIP , XI_OFF);
    HandleResult(mStat,"xiSetParam (Flip vertical  - on)");

	mOpened = true;
    printf("Camer Open succeed!\n");
}

XIMEAVideo::~XIMEAVideo()
{
	if(mOpened){
		xiStopAcquisition(mXiH);
    	xiCloseDevice(mXiH);
	}
}

XIMEAVideo& XIMEAVideo::operator>>(cv::Mat& image)
{
	mStat = xiGetImage(mXiH, 5000, &mImage);
	if(mStat != XI_OK){
		printf("xiGetImage failed!\n");
		image = cv::Mat();
	}else{
		image = cv::Mat(cv::Size(2048,1088), CV_8UC1, mImage.bp);
	}

	return *this;
}