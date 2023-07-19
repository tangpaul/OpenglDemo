//#include <GL/glew.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Utilities.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Camera.h"

#include "glm/glm.hpp" 
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "tests/TestClearColor.h"
#include "tests/TestUdpMonitor.h"

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

// camera
Camera camera(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f), 45.0f, -135.0f);
float lastX = (float) 1920 / 2.0;
float lastY = (float) 1080 / 2.0;
float curX = 0.0f, curY = 0.0f;
bool is_pressed = false;

//timing
float deltaTime = 0.0f, lastFrame = 0.0f;


int main(void){
	GLFWwindow* window;

	/* Initialize the library */
	if(!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	//glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	/* Create a windowed mode window and its OpenGL context */
	//window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	window = glfwCreateWindow(960,
								540, "MyWindow",
							  nullptr, nullptr);
	if(!window){
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!status) {
		std::cout << "Glad Error!" << std::endl;
	}

	glfwSwapInterval(1);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_btn_callback);
	std::cout << glGetString(GL_VERSION) << std::endl;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	{
		Renderer renderer;

		//imgui
		// Setup Dear ImGui context
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void) io;
		{ 
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

			ImGuiStyle& style = ImGui::GetStyle();
			if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}
		}
		ImPlot::CreateContext();
		ImGui::StyleColorsDark();
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestUdpMonitor>("Udp Monitor");
		//testMenu->RegisterTest<test::TestTexture3DCube>("3D Cube Texture");
		//testMenu->RegisterTest <test::TestTexture3DLines>("Gyro Watch");
		//testMenu->RegisterTest <test::TestCamTracking>("Cam Track");

		testMenu->StartWith(1);

		// timing
		/* Loop until the user closes the window */
		while(!glfwWindowShouldClose(window)){
			// per-frame time logic
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			//input
			processInput(window);

			/* Render here */
			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			renderer.Clear();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			if (currentTest)
			{
				currentTest->OnUpdate(0.0f);
				currentTest->OnCameraUpdate(camera.GetViewMatrix(), camera.Zoom);
				currentTest->OnRender();
				ImGui::Begin("Tests");
				if (currentTest != testMenu && ImGui::Button("<--"))
				{
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			//imgui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		if (currentTest != testMenu)
		{
			delete testMenu;
		}
		delete currentTest;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	const ImGuiIO& io = ImGui::GetIO();
	if(io.WantCaptureMouse)return;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	const ImGuiIO& io = ImGui::GetIO();
	if(io.WantCaptureMouse)return;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (is_pressed)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	curX = xpos;
	curY = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	const ImGuiIO& io = ImGui::GetIO();
	if(io.WantCaptureMouse)return;

	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_btn_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		lastX = curX;
		lastY = curY;
		is_pressed = true;
	}
	else{
		is_pressed = false;
	}
}