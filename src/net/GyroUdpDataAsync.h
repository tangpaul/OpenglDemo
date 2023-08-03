#pragma once

#include <array>
#include <thread>
#include <future>

class IGyroUdpDataAsyncListener{
public:
	virtual void AsyncData(const float timestamp, const float* data) = 0;
};

class GyroUdpDataAsync {
private:
	const int buffer_lines_size		= 0;
	const int async_data_size		= 0;
	const int m_fps = 100;
	float fpsInterval = 1.0f / (float) m_fps;

	float async_stable_time = 1000.0f / 100.0f;		//同步稳定时间 ms
	float* buffer	= nullptr;	//缓存
	volatile unsigned long long current_product_num = 0;	//当前生产数量
	volatile unsigned long long current_consume_num = 0;	//当前消费数量

	volatile bool stopAsyncThread = false;		//停止同步线程
	volatile bool startConsume = false;			//开始消费

	IGyroUdpDataAsyncListener* listener = nullptr;

public:
	GyroUdpDataAsync(int datasize, int buffersize, int fps = 100) ;
	~GyroUdpDataAsync();

public:
	void product(const float timestamp, const float* data);
	void startAsync();
	void stopAsync();
	void registerListener(IGyroUdpDataAsyncListener* host);
};