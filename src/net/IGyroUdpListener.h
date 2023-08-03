#pragma once
#include <fstream>
#include <string>
#ifdef __linux__
#include <m3api/wintypedefs.h>
#endif
// #ifdef __linux__
// #ifdef __cplusplus
// extern "C" {
// #endif
// typedef signed char         INT8, *PINT8;
// typedef signed short        INT16, *PINT16;
// typedef signed int          INT32, *PINT32;
// typedef signed long long int      	INT64, *PINT64;
// typedef unsigned char       UINT8, *PUINT8;
// typedef unsigned short      UINT16, *PUINT16;
// typedef unsigned int        UINT32, *PUINT32;
// typedef unsigned long long int    	UINT64, *PUINT64;
// #ifdef __cplusplus
// }
// #endif
// #endif // DEBUG

class IGyroUdpListener{
public:
	virtual void reciveQuat(float w, float x, float y, float z) = 0;
	virtual void reciveTimeCode(uint8_t timecode1, uint8_t timecode2, uint8_t timecode3, uint8_t timecode4) = 0;
	virtual void recive3AxisParams( float timestamp,
									float w1, float w2, float w3,
									float aw1, float aw2, float aw3,
									float a1, float a2, float a3) = 0;
	virtual void reciveError(float error)	= 0;
	virtual void saveALine(std::ofstream& outfile, const std::string timestamp)	= 0;

	virtual void recive3AxisParamsStable(float timestamp, float w1, float w2, float w3,
										float aw1, float aw2, float aw3,
										float a1, float a2, float a3) = 0;
};