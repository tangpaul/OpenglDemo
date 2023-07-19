#include "TestUdpMonitor.h"

#include "Utilities.h"
#include "imgui.h"
#include "implot.h"
#include "imgui_memory_editor.h"

#include <iostream>
#include <thread>
#include <cstring>
#include <sys/types.h>
#include <memory>
#include <queue>

#ifndef __linux__
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif 

namespace test
{
	const int BUFFER_SIZE = 1024;
	const int PORT = 6311;
	const int PORT2 = 6312;
	static MemoryEditor mem_edit;
	TestUdpMonitor::TestUdpMonitor()
		:m_ClearColor{ 0.2f,0.6f,0.8f,1.0f }
	{
		m_Buffer = new char[BUFFER_SIZE];
		memset(m_Buffer, 0x00, BUFFER_SIZE * sizeof(char));
		m_Buffer2 = new char[BUFFER_SIZE];
		memset(m_Buffer2, 0x00, BUFFER_SIZE * sizeof(char));

		// 初始化winsock
#ifndef __linux__
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cout << "WSAStartup failed with error" << std::endl;
			return;
		}
#endif 

		// 创建接收socket
		m_Socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		m_Socketfd2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(m_Socketfd < 0 || m_Socketfd2 < 0){
			std::cout << "socket error" << std::endl;
			return;
		}

		// 设置接收超时
		int timeout = 3000; // 超时时间设为3000毫秒
		if (setsockopt(m_Socketfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
			std::cerr << "setsockopt for SO_RCVTIMEO failed with error" << std::endl;
			return;
		}
		if (setsockopt(m_Socketfd2, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
			std::cerr << "setsockopt for SO_RCVTIMEO failed with error" << std::endl;
			return;
		}

		// 设置服务器地址结构
		sockaddr_in server_addr;
		std::memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(PORT);

		// 绑定socket和地址
		if(bind(m_Socketfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			std::cout << "bind error" << std::endl;
			closesocket(m_Socketfd);
			WSACleanup();
			return;
		}
		server_addr.sin_port = htons(PORT2);
		if (bind(m_Socketfd2, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			std::cout << "bind error" << std::endl;
			closesocket(m_Socketfd2);
			WSACleanup();
			return;
		}

		m_UdpMonitorFuture = std::async(&TestUdpMonitor::UdpMonitorStart, this);
		m_UdpMonitorFuture2 = std::async(&TestUdpMonitor::UdpMonitorStart2, this);
	}

	TestUdpMonitor::~TestUdpMonitor(){
		UdpMonitorStop();
		m_UdpMonitorFuture.wait();
		UdpMonitorStop2();
		m_UdpMonitorFuture2.wait();

		if (m_Socketfd > 0)
		{
			closesocket(m_Socketfd);
		}
		if (m_Socketfd2 > 0)
		{
			closesocket(m_Socketfd2);
		}
		WSACleanup();

		if(m_Buffer != nullptr)
			delete[] m_Buffer;
		if (m_Buffer2 != nullptr)
			delete[] m_Buffer2;
	}

	void TestUdpMonitor::OnUpdate(float deltaTime){

	}

	void TestUdpMonitor::OnRender(){
		GLCall(glClearColor(m_ClearColor[0],m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestUdpMonitor::OnImGuiRender(){
		ImGui::Text("you can exit by clicking the button upon.");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		//udp监视窗体
		// ImGui::SetNextWindowSize(ImVec2(600, 500));
		ImGui::SetNextWindowPos(ImVec2(20, 20));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
		if(ImGui::Begin("Monitor", nullptr, window_flags))
		{
			ImGui::Text("This window is for monitoring the UDP packets.");
			ImGui::Text("real time interval: %.3f ms", m_timeInterval2);
			ImGui::Text("");

			//ImGui::Text("Bad package count: %d  (lower than 8.5ms or higher than 11.5ms)", m_BadIntervalCount);
			static ImPlotAxisFlags flags = ImPlotAxisFlags_None;
			if(ImPlot::BeginPlot("Ticks", ImVec2(-1, 400))){
				ImPlot::SetupAxes(NULL, NULL, flags, flags | ImPlotAxisFlags_AutoFit);
				ImPlot::SetupAxisLimits(ImAxis_X1, m_ticksCount2 - 2000, m_ticksCount2, m_IsRunning2 ? ImGuiCond_Always : ImGuiCond_Once);
				ImPlot::SetNextLineStyle(ImVec4(0.f, 1.f, 0.f, 1.f), 0.5f); 
				ImPlot::PlotLine("interval", &m_ticksBuffer2.Data[0].x, &m_ticksBuffer2.Data[0].y, m_ticksBuffer2.Data.size(), 0, m_ticksBuffer2.Offset, 2 * sizeof(float));
				ImPlot::EndPlot();
			}

			if(m_IsRunning || m_IsRunning2){
				if(ImGui::Button("Stop")){
					UdpMonitorStop();
					m_UdpMonitorFuture.wait();
					UdpMonitorStop2();
					m_UdpMonitorFuture2.wait();
				}
			} else {
				if(ImGui::Button("Start")){
					m_UdpMonitorFuture = std::async(&TestUdpMonitor::UdpMonitorStart, this);
					m_UdpMonitorFuture2 = std::async(&TestUdpMonitor::UdpMonitorStart2, this);
				}
			}

			ImGui::End();
		}
		mem_edit.DrawWindow("Memory Editor", m_Buffer, BUFFER_SIZE);
	}

	void TestUdpMonitor::UdpMonitorStart(){
		std::cout << "UDP server listening on port " << PORT << std::endl;

		m_IsRunning = true;
		m_ticksCount = 0;
		m_ticksBuffer = ScrollingBuffer(2000);
		m_PrevTime = std::chrono::high_resolution_clock::now();

		while (m_IsRunning) {
			sockaddr_in client_addr;
			int client_addr_len = sizeof(sockaddr_in);
			int recv_len = recvfrom(m_Socketfd, m_Buffer, BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_addr_len);

			if (recv_len > 0) {
				if(recv_len < BUFFER_SIZE)
					memset(m_Buffer + recv_len, 0x00, BUFFER_SIZE - recv_len);

				//记下时间戳
				m_PrevTime = std::chrono::high_resolution_clock::now();
				m_timeStamp_us = CastTimeToMicroseconds(m_PrevTime);
			} else {
				std::cout << "Error: recvfrom failed" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		std::cout << "UDP server stop listening."<< std::endl;
	}

	void TestUdpMonitor::UdpMonitorStop(){
		m_IsRunning = false;
	}

	void TestUdpMonitor::UdpMonitorStart2()
	{
		std::cout << "UDP server2 listening on port " << PORT2 << std::endl;

		m_file.open(Utility::GetDateTimeStr() + ".txt", std::ios::out | std::ios::app);
		if (!m_file.is_open()) {
			std::cerr << "Failed to open File." << std::endl;
			return;
		}

		m_IsRunning2 = true;
		m_ticksCount2 = 0;
		m_ticksBuffer2 = ScrollingBuffer(2000);
		m_NowTime = std::chrono::high_resolution_clock::now();

		while (m_IsRunning2) {
			sockaddr_in client_addr;
			int client_addr_len = sizeof(sockaddr_in);
			int recv_len = recvfrom(m_Socketfd2, m_Buffer2, BUFFER_SIZE, 0, (sockaddr*)&client_addr, &client_addr_len);

			if (recv_len > 0) {
				if (recv_len < BUFFER_SIZE)
					memset(m_Buffer2 + recv_len, 0x00, BUFFER_SIZE - recv_len);

				// 计算时间间隔
				m_NowTime = std::chrono::high_resolution_clock::now();
				m_timeStamp_us2 = CastTimeToMicroseconds(m_NowTime);
				m_timeInterval2 = (m_timeStamp_us2 - m_timeStamp_us) / 1000.0f;
				m_ticksBuffer2.AddPoint(m_ticksCount2++, m_timeInterval2);

				////写入文件
				char line[256] = { NULL };
				sprintf_s(line, "%d,%.3f\n", m_ticksCount2, m_timeInterval2);
				m_file << line;
			}
			else {
				std::cout << "Error: recvfrom2 failed" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		m_file.close();
		std::cout << "UDP server2 stop listening." << std::endl;
	}

	void TestUdpMonitor::UdpMonitorStop2()
	{
		m_IsRunning2 = false;
	}

	long TestUdpMonitor::CastTimeToMicroseconds(const std::chrono::high_resolution_clock::time_point& time)
	{
		auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(time);
		auto epoch = now_us.time_since_epoch();
		auto value = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
		long duration = value.count();

		return duration;
	}
}
