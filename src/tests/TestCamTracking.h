#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

#include "glm/glm.hpp" 
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>

#include "implot.h"
#include "implot_sturcts.h"

#include <memory>
#include <future>
#include <thread>
#include <queue>
#include <mutex>

#include "GyroUdpCapture.h"
#include "ITrackerListener.h"

#include <opencv2/opencv.hpp>

namespace test
{
	class TestCamTracking : public Test , public IGyroUdpListener
	{
	public:
		TestCamTracking();
		~TestCamTracking();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender()override;

		//gyro
		virtual void reciveQuat(float w, float x, float y, float z) ;
		virtual void reciveTimeCode(uint8_t timecode1, uint8_t timecode2, uint8_t timecode3, uint8_t timecode4){};
		virtual void recive3AxisParams( float timestamp,
										float w1, float w2, float w3,
										float aw1, float aw2, float aw3,
										float a1, float a2, float a3){};
		virtual void reciveError(float error){};
		virtual void saveALine(std::ofstream& outfile, const std::string timestamp){};

		virtual void recive3AxisParamsStable(float timestamp, float w1, float w2, float w3,
											float aw1, float aw2, float aw3,
											float a1, float a2, float a3){};

	private:
		void startUdpCapture();
		void stopUdpCapture();

		void startTracking();

		void captureFrame();

	private:
		//模型坐标系
		std::unique_ptr<VertexArray>	m_VAO;
		std::unique_ptr<VertexBuffer>	m_VBO;
		std::unique_ptr<IndexBuffer>	m_IndexBuffer;
		std::unique_ptr<Shader>			m_Shader;
		float m_RValues[3] = { 0.0f };
		glm::quat m_Quat;

		//模型坐标系
		std::unique_ptr<VertexArray>	m_VAO2;
		std::unique_ptr<VertexBuffer>	m_VBO2;
		std::unique_ptr<IndexBuffer>	m_IndexBuffer2;
		float m_RValues2[3] = { 0.0f };
		glm::quat m_Quat2;

		//世界坐标系
		std::unique_ptr<VertexArray>	m_VAOAxis;
		std::unique_ptr<VertexBuffer>	m_VBOAxis;
		std::unique_ptr<IndexBuffer>	m_IndexBufferAxis;

		//地面
		std::unique_ptr<VertexArray>	m_VAOGround;
		std::unique_ptr<VertexBuffer>	m_VBOGround;
		std::unique_ptr<IndexBuffer>	m_IndexBufferGround;
		std::unique_ptr<Shader>			m_ShaderGround;
		std::unique_ptr<Texture>		m_Texutre;

		glm::mat4 m_Proj, m_View;

		float m_ViewDistance	= 1.0f;

		volatile bool m_ForceStop = false;
		std::future<void>	m_XIMEACameraFuture;

		//IMU GYRO
		std::unique_ptr<GyroUdpCapture> m_GyroUdpATraceCapture;
		std::future<void>	m_GyroUdpATraceFuture;
		std::vector<const char*>	m_devNames;
		int m_devATraceCurrent = 0;

		//Capture
		volatile int m_CaptureIndex = 0;
		cv::Mat m_CaptureFrame;
		std::mutex m_CaptureFrameMutex;
	};
}