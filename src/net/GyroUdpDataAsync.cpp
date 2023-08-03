#include "GyroUdpDataAsync.h"
#ifdef __linux__
#include <unistd.h>
#include <string.h>
#endif

GyroUdpDataAsync::GyroUdpDataAsync(int datasize, int buffersize, int fps) :
	buffer_lines_size(buffersize), async_data_size(datasize), m_fps(fps)
{
	buffer = new float[(async_data_size + 1) * buffer_lines_size];

	fpsInterval = 1.0f / (float) m_fps;
}

GyroUdpDataAsync::~GyroUdpDataAsync(){
	if(buffer != nullptr) delete[] buffer;
}

void GyroUdpDataAsync::product(const float timestamp, const float* data){
	unsigned int offset = current_product_num % buffer_lines_size;
	float* pDataProduct = buffer + offset * (async_data_size + 1);
	pDataProduct[0] = timestamp;
	memcpy(pDataProduct + 1, data, async_data_size * sizeof(float));
	current_product_num++;
}

void GyroUdpDataAsync::startAsync(){
	stopAsyncThread = false;

	auto last = std::chrono::high_resolution_clock::now();

	while(!stopAsyncThread)	{

		if(current_product_num - current_consume_num > 20)
			startConsume = true;
		
		if (startConsume)
		{
			unsigned int offset = current_consume_num % buffer_lines_size;
			float* consumeData = buffer + offset * (async_data_size + 1);;
			if(listener != nullptr) listener->AsyncData(consumeData[0], consumeData + 1);
			current_consume_num++;

			if(current_product_num - current_consume_num <= 0) startConsume = false;
		}

		//稳定延时，保证帧率
		auto target = last + std::chrono::microseconds((long)(fpsInterval * 1000000));
		while(std::chrono::high_resolution_clock::now() < target)	{
		#ifdef __linux__
			usleep(500);//0.5ms
		#endif
		}
		last = std::chrono::high_resolution_clock::now();
	}
}

void GyroUdpDataAsync::stopAsync(){
	stopAsyncThread = true;
}

void GyroUdpDataAsync::registerListener(IGyroUdpDataAsyncListener* host){
	listener = host;
}
