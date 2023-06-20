#include "TestClearColor.h"

#include "Utilities.h"
#include "imgui.h"

namespace test
{


	TestClearColor::TestClearColor()
		:m_ClearColor{ 0.2f,0.6f,0.8f,1.0f }	
	{

	}

	TestClearColor::~TestClearColor(){

	}

	void TestClearColor::OnUpdate(float deltaTime){

	}

	void TestClearColor::OnRender(){
		static bool isClear = true;

		if (isClear)
		{
			GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));
		}
		else 
		{
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));
		}
		isClear = !isClear;
	}

	void TestClearColor::OnImGuiRender(){
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}
