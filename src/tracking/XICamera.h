#pragma once

#include <opencv2/opencv.hpp>
#ifdef __linux__
#include <m3api/xiApi.h>
#else
#include <xiApi.h>
#endif

namespace TKParams
{
	extern cv::Mat cameraMatrix;

	extern cv::Mat distCoeffs;
}

class XIMEAVideo
{
public:
	XIMEAVideo();
	~XIMEAVideo();

	inline bool isOpened() {return mOpened;}

	XIMEAVideo& operator>>(cv::Mat& image);

private:


private:
	HANDLE mXiH = NULL;			//相机句柄
	XI_RETURN mStat = XI_OK;	//返回值
	XI_IMG mImage;				//图像

	bool mOpened	= false;	//相机是否打开
};

