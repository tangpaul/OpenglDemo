#pragma once

#include "Test.h"

#include "implot_sturcts.h"

#include <future>
#include <chrono>
#include <iostream>
#include <fstream>

namespace test
{
	class TestUdpMonitor : public Test	{
	public:
		TestUdpMonitor();
		~TestUdpMonitor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender()override;

	private:
		void UdpMonitorStart();
		void UdpMonitorStop();

		void UdpMonitorStart2();
		void UdpMonitorStop2();

	private:
		long CastTimeToMicroseconds(const std::chrono::high_resolution_clock::time_point& time);

	private:
		float m_ClearColor[4];

		int m_Socketfd = -1;
		std::future<void>	m_UdpMonitorFuture;
		char* m_Buffer = nullptr;

		volatile bool m_IsRunning = false;
		ScrollingBuffer m_ticksBuffer;
		volatile int m_ticksCount = 0;
		volatile long m_timeStamp_us = 0;

		int m_Socketfd2 = -1;
		std::future<void>	m_UdpMonitorFuture2;
		char* m_Buffer2 = nullptr;

		volatile bool m_IsRunning2 = false;
		ScrollingBuffer m_ticksBuffer2;
		volatile int m_ticksCount2 = 0;
		volatile long m_timeStamp_us2 = 0;
		volatile float m_timeInterval2 = 0.0f;

		std::chrono::high_resolution_clock::time_point m_PrevTime;
		std::chrono::high_resolution_clock::time_point m_NowTime;

		volatile long long m_BadIntervalCount = 0;
		std::ofstream m_file;
	};
}
