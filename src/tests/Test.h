#pragma once

#include <vector>
#include <string>
#include <functional>

#include <glm/glm.hpp>

namespace test
{

	class Test{
	public:
		Test(){ }
		virtual ~Test(){ }

		virtual void OnUpdate(float deltaTime){ }
		virtual void OnCameraUpdate(glm::mat4 view, float zoom){ }
		virtual void OnRender(){ }
		virtual void OnImGuiRender(){ }
	};

	class TestMenu : public Test	{
	public:
		TestMenu(Test*& currentTestPointer);
		~TestMenu();

		void OnImGuiRender() override;

		template <typename T>
		void RegisterTest(const std::string& name)
		{
			m_Tests.push_back(std::make_pair(name, [](){ return new T(); }));
		}

		void StartWith(int i) 
		{
			auto it = m_Tests[i];
			m_CurrentTest = it.second();
		}

	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}