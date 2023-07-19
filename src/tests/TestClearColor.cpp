#include "TestClearColor.h"

#include "Utilities.h"
#include "imgui.h"
#include "imgui_memory_editor.h"


namespace test
{
	static MemoryEditor mem_edit;	//ÄÚ´æ±à¼­Æ÷

	TestClearColor::TestClearColor()
		:m_ClearColor{ 0.2f,0.6f,0.8f,1.0f }	
	{
		for (int i = 0; i < 1024; i++)
		{
			m_MemoryBuffer[i] = i % 255;
		}
	}

	TestClearColor::~TestClearColor(){

	}

	void TestClearColor::OnUpdate(float deltaTime){

	}

	void TestClearColor::OnRender(){
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestClearColor::OnImGuiRender(){
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		mem_edit.DrawContents(m_MemoryBuffer, 256);
	}

}
