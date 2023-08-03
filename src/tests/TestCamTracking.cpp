#include "TestCamTracking.h"

#include "Utilities.h"
#include "imgui.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/euler_angles.hpp>

#include "Tracker.h"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace test
{
	TestCamTracking::TestCamTracking() :
		m_Proj(glm::perspective(glm::radians(45.0f), (float) 1920 / (float) 1080, 0.1f, 100.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f))),
		m_Quat(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
		m_Quat2(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
	{
		GLCall(glEnable(GL_BLEND));										//开启混合模式
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));		//混合Alpha属性设置
		GLCall(glEnable(GL_DEPTH_TEST));								//开启深度测试

		//3 轴带箭头坐标系
		float position[] = {	//position  color
			0.0f,   0.0f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//z red
			0.0f,   0.0f,   1.0f,		1.0f, 0.0f, 0.0f, 1.0f,	//z red
		   0.05f,   0.0f,   0.9f,		1.0f, 0.0f, 0.0f, 1.0f,	//z red
		  -0.05f,   0.0f,   0.9f,		1.0f, 0.0f, 0.0f, 1.0f,	//z red

			0.0f,   0.0f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//y green
			0.0f,   1.0f,   0.0f,		0.0f, 1.0f, 0.0f, 1.0f,	//y green
		   0.05f,   0.9f,   0.0f,		0.0f, 1.0f, 0.0f, 1.0f,	//y green
		  -0.05f,   0.9f,   0.0f,		0.0f, 1.0f, 0.0f, 1.0f,	//y green

			0.0f,   0.0f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//x blue
			1.0f,   0.0f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//x blue
			0.9f,  0.05f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//x arrow
			0.9f, -0.05f,   0.0f,		0.0f, 0.0f, 1.0f, 1.0f,	//x arrow
		};
		unsigned int indices[] = {
			0,  1,
			1,  2,
			1,  3,

			4,  5,
			5,  6,
			5,  7,

			8,  9,
			9,  10,
			9,  11,
		};
		m_VAO = std::make_unique<VertexArray>();

		m_VBO = std::make_unique<VertexBuffer>(position, 7 * 4 * 3 * sizeof(float));
		VertexBufferLayout layout;
		layout.add<float>(3);
		layout.add<float>(4);
		m_VAO->AddBuffer(*m_VBO, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6 * 3);

		//3 轴带箭头坐标系 2
		float position2[] = {	//position  color
			0.0f,   0.0f,   0.0f,		1.0f, 0.0f, 0.0f, 1.0f,	//z red
			0.0f,   0.0f,   1.0f,		0.8f, 0.3f, 0.2f, 1.0f,	//z red
			0.0f,  0.05f,   0.9f,		0.8f, 0.3f, 0.2f, 1.0f,	//z red
			0.0f, -0.05f,   0.9f,		0.8f, 0.3f, 0.2f, 1.0f,	//z red

			0.0f,   0.0f,   0.0f,		1.0f, 0.0f, 0.0f, 1.0f,	//y green
			0.0f,   1.0f,   0.0f,		0.3f, 0.8f, 0.2f, 1.0f,	//y green
			0.0f,   0.9f,  0.05f,		0.3f, 0.8f, 0.2f, 1.0f,	//y green
			0.0f,   0.9f, -0.05f,		0.3f, 0.8f, 0.2f, 1.0f,	//y green

			0.0f,   0.0f,   0.0f,		1.0f, 0.0f, 0.0f, 1.0f,	//x blue
			1.0f,   0.0f,   0.0f,		0.3f, 0.2f, 0.8f, 1.0f,	//x blue
			0.9f,	0.0f,  0.05f,		0.3f, 0.2f, 0.8f, 1.0f,	//x arrow
			0.9f,	0.0f, -0.05f,		0.3f, 0.2f, 0.8f, 1.0f,	//x arrow
		};
		unsigned int indices2[] = {
			0,  1,
			1,  2,
			1,  3,
			2,	3,

			4,  5,
			5,  6,
			5,  7,
			6,  7,

			8,  9,
			9,  10,
			9,  11,
			10,  11,
		};
		m_VAO2 = std::make_unique<VertexArray>();

		m_VBO2 = std::make_unique<VertexBuffer>(position2, 7 * 4 * 3 * sizeof(float));
		VertexBufferLayout layout2;
		layout2.add<float>(3);
		layout2.add<float>(4);
		m_VAO2->AddBuffer(*m_VBO2, layout2);

		m_IndexBuffer2 = std::make_unique<IndexBuffer>(indices2, 8 * 3);


		//3 轴世界坐标系
		float aixsPosition[] = {
			0.0f,   0.0f,   0.0f,		1.0f, 0.0f, 0.0f, 0.5f,	//z red
			0.0f,   0.0f,   10.0f,		1.0f, 0.0f, 0.0f, 0.5f,	//z red

			0.0f,   0.0f,   0.0f,		0.0f, 1.0f, 0.0f, 0.5f,	//y green
			0.0f,   10.0f,  0.0f,		0.0f, 1.0f, 0.0f, 0.5f,	//y green

			0.0f,   0.0f,   0.0f,		0.0f, 0.0f, 1.0f, 0.5f,	//x blue
			10.0f,  0.0f,  0.0f,		0.0f, 0.0f, 1.0f, 0.5f,	//x blue
		};
		unsigned int aixsIndices[] = {
			0,  1,
			2,  3,
			4,  5,
		};
		m_VAOAxis = std::make_unique<VertexArray>();

		m_VBOAxis = std::make_unique<VertexBuffer>(aixsPosition, 7 * 2 * 3 * sizeof(float));
		m_VAOAxis->AddBuffer(*m_VBOAxis, layout);

		m_IndexBufferAxis = std::make_unique<IndexBuffer>(aixsIndices, 2 * 3);

		//着色器
		m_Shader = std::make_unique<Shader>("res/shaders/Basic2.shader");
		m_Shader->Bind();

		//地面
		float goundPosition[20] = {
			-10.0f, -10.0f,  0.0f,		0.0f, 0.0f,
			 10.0f, -10.0f,  0.0f,		1.0f, 0.0f,
			 10.0f,  10.0f,  0.0f,		1.0f, 1.0f,
			-10.0f,  10.0f,  0.0f,		0.0f, 1.0f
		};
		unsigned int goundIndices[] = {
			0,  1,  2,
			2,  3,  0
		};
		m_VAOGround = std::make_unique<VertexArray>();

		m_VBOGround = std::make_unique<VertexBuffer>(goundPosition, 5 * 4 * sizeof(float));
		VertexBufferLayout groundLayout;
		groundLayout.add<float>(3);
		groundLayout.add<float>(2);
		m_VAOGround->AddBuffer(*m_VBOGround, groundLayout);

		m_IndexBufferGround = std::make_unique<IndexBuffer>(goundIndices, 3 * 2);

		//地面着色器
		m_ShaderGround = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_ShaderGround->Bind();

		m_Texutre = std::make_unique<Texture>("res/images/marble3.png");
		m_Texutre->Bind(0);
		m_ShaderGround->SetUniform1i("u_Texture", 0);

		m_Texutre->Unbind();

		//Gyro
		m_GyroUdpATraceCapture = std::make_unique<GyroUdpCapture>(110);
		m_GyroUdpATraceCapture->RegisterListener(this);
		const std::vector<std::string>& devNameStr = m_GyroUdpATraceCapture->GetDevNames();
		for(int i = 0; i < devNameStr.size(); i++){
			m_devNames.push_back(devNameStr[i].c_str());
		}
	}

	TestCamTracking::~TestCamTracking(){
		if((m_GyroUdpATraceFuture.valid() && m_GyroUdpATraceFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) ||
		   (m_XIMEACameraFuture.valid() && m_XIMEACameraFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)){
			this->stopUdpCapture();
		}
	}

	void TestCamTracking::OnUpdate(float deltaTime){

	}

	void TestCamTracking::OnRender(){
		GLCall(glClearColor(0.2f, 0.6f, 0.8f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_View = glm::lookAt(glm::vec3(m_ViewDistance, m_ViewDistance, m_ViewDistance), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); //45度角看向原点, z轴朝上

		Renderer renderer;

		//画3轴带箭头坐标系
		glm::mat4 model = glm::toMat4(m_Quat);
		float eulerX = 0.0f, eulerY = 0.0f, eulerZ = 0.0f;
		glm::extractEulerAngleXYZ(model, eulerX, eulerY, eulerZ);
		m_RValues[0] = glm::degrees(eulerX);
		m_RValues[1] = glm::degrees(eulerY);
		m_RValues[2] = glm::degrees(eulerZ);
		glm::mat4 mvp = m_Proj * m_View * model;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp);

		renderer.DrawLines(*m_VAO, *m_IndexBuffer, *m_Shader, 4.0f);

		//画3轴带箭头坐标系2
		glm::mat4 model2 = glm::toMat4(m_Quat2);
		glm::extractEulerAngleXYZ(model2, eulerX, eulerY, eulerZ);
		m_RValues2[0] = glm::degrees(eulerX);
		m_RValues2[1] = glm::degrees(eulerY);
		m_RValues2[2] = glm::degrees(eulerZ);
		glm::mat4 mvp2 = m_Proj * m_View * model2;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp2);

		renderer.DrawLines(*m_VAO2, *m_IndexBuffer2, *m_Shader, 4.0f);

		//画3轴世界坐标系
		glm::mat4 modelAxis = glm::mat4(1.0f);
		glm::mat4 mvpAxis = m_Proj * m_View * modelAxis;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvpAxis);

		renderer.DrawLines(*m_VAOAxis, *m_IndexBufferAxis, *m_Shader, 1.5f);

		//画出地面
		m_Texutre->Bind(0);
		m_ShaderGround->Bind();
		m_ShaderGround->SetUniformMat4f("u_MVP", mvpAxis);

		renderer.Draw(*m_VAOGround, *m_IndexBufferGround, *m_ShaderGround);
	}

	void TestCamTracking::OnImGuiRender(){
		//基本信息
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text(" ");

		//Gyro网卡
		ImGui::Text("adapter ATrace [%d]:", m_devATraceCurrent);	ImGui::SameLine();	ImGui::SetNextItemWidth(450);	ImGui::Combo(" ", &m_devATraceCurrent, &m_devNames[0], m_devNames.size());
		ImGui::Text(" ");

		//位姿信息
		ImGui::Text("Position Info ATrace");
		ImGui::SliderFloat3("rotation degrees", m_RValues, -180.0f, 180.0f);
		ImGui::SliderFloat4("quat x y z w", &m_Quat2.x, -1.0f, 1.0f);
		ImGui::Text(" ");

		//视图控制
		ImGui::Text("View control");
		ImGui::SliderFloat("view distance", &m_ViewDistance, 1.0f, 10.0f);
		ImGui::Text(" ");

		//追踪按钮
		if(m_GyroUdpATraceFuture.valid() && m_GyroUdpATraceFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready){
			if(ImGui::Button("Stop Track")){
				this->stopUdpCapture();
			}
		}
		else{
			if(ImGui::Button("Start Track")){
				this->startUdpCapture();
			}
		}

		//捕获界面
		ImGui::Text("");
		ImGui::Text("Capture");
		if (ImGui::Button("capture frame"))
		{
			this->captureFrame();
		}
		ImGui::Text(" ");
		ImGui::Text("Captured index: %d", m_CaptureIndex);
	}

	void TestCamTracking::reciveQuat(float w, float x, float y, float z) {
		m_Quat2.x = x;
		m_Quat2.y = y;
		m_Quat2.z = z;
		m_Quat2.w = w;
	}

	void TestCamTracking::startUdpCapture(){
		m_GyroUdpATraceFuture = std::async(&GyroUdpCapture::StartListen, m_GyroUdpATraceCapture.get(), m_GyroUdpATraceCapture->GetDevNames()[m_devATraceCurrent], "ATrace");
		m_XIMEACameraFuture = std::async(&TestCamTracking::startTracking, this);
	}

	void TestCamTracking::stopUdpCapture(){
		m_CaptureIndex = 0;
		m_ForceStop = true;
		m_XIMEACameraFuture.wait();
		m_GyroUdpATraceCapture->Stop();
		m_GyroUdpATraceFuture.wait();
	}

	void TestCamTracking::startTracking()
	{
		using namespace cv;

		//读取视频
#ifdef __linux__
		XIMEAVideo cap;
		if(!cap.isOpened()){
			std::cout << "open video failed." << std::endl;
			return;
		}
#else
		VideoCapture cap;
		if(!cap.open("H:/temp/Calibration/20221207_video/points2.avi")){
			std::cout << "open video failed" << std::endl;
			return;
		}
#endif // DEBUG
		std::cout << "open video succeed." << std::endl;
		
		//帧循环
		Mat rsframe;
		while(!m_ForceStop){
			{
				std::lock_guard<std::mutex> lock(m_CaptureFrameMutex);
				cap >> m_CaptureFrame;
			}

			if(m_CaptureFrame.empty()){
				std::cout << "get empty frame.\n";
				break;
			}
	
			resize(m_CaptureFrame, rsframe, Size(1024, 544));
			imshow("video", rsframe);
			waitKey(1);
		}
		std::cout << "video finished.\n";
		cv::destroyWindow("video");
	}

	void TestCamTracking::captureFrame()
	{
		static std::string DateStr = "default";
		if (m_CaptureIndex == 0) {
			// 获取当前时间点
			auto now = std::chrono::system_clock::now();
			// 转换为time_t来使用C标准库
			auto now_c = std::chrono::system_clock::to_time_t(now);
			// 转换为字符串
			std::ostringstream oss;
			oss << std::put_time(std::localtime(&now_c), "%Y%m%d%H%M%S");
			DateStr = oss.str();
		}
		std::filesystem::path camDirPath("./" + DateStr + "/cam");
		std::filesystem::path imuDirPath("./" + DateStr + "/imu");
		if (!std::filesystem::exists(camDirPath)) {
			std::filesystem::create_directories(camDirPath);
		}
		if (!std::filesystem::exists(imuDirPath)) {
			std::filesystem::create_directories(imuDirPath);
		}

		//Capture Frame
		std::string camFileName = "./" + DateStr + "/cam/" + std::to_string(m_CaptureIndex) + ".png";
		std::string imuFileName = "./" + DateStr + "/imu/" + std::to_string(m_CaptureIndex) + ".quat";

		{
			std::lock_guard<std::mutex> lock(m_CaptureFrameMutex);

			if (!cv::imwrite(camFileName, m_CaptureFrame))
				std::cout << "save frame failed.\n";
		}

		std::ofstream imuFile(imuFileName, std::ios::binary);
		if(!imuFile.is_open())
			std::cout << "save imu data failed.\n";

		float data[4] = { m_Quat2.w, m_Quat2.x, m_Quat2.y, m_Quat2.z };
		imuFile.write((char*)data, sizeof(data));
		imuFile.close();

		m_CaptureIndex++;
	}
}
