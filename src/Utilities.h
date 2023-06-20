#pragma once

#include "glad/glad.h"
#include <iostream>
#include <chrono>
//#include <Eigen/Dense>

#ifdef __linux__
#define ASSERT(x) if(!(x)) throw;
#else
#define ASSERT(x) if(!(x)) __debugbreak();
#endif // DEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

namespace Utility
{
    struct Timer
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
        std::string description;

        Timer(std::string des = "")
        {
            description = des;
            startTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~Timer()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            auto duration = end - start;

            double ms = duration * 0.001;
            if(description.empty())
                std::cout << "time cost: " << ms << "ms\n";
            else
                std::cout << "[" << description << "] " << "time cost: " << ms << "ms\n";
        }
    }; 

    //template <typename Derived>
    //static Eigen::Matrix<typename Derived::Scalar, 3, 3> skewSymmetric(const Eigen::MatrixBase<Derived> &q)
    //{
    //    Eigen::Matrix<typename Derived::Scalar, 3, 3> ans;
    //    ans << typename Derived::Scalar(0), -q(2), q(1),
    //        q(2), typename Derived::Scalar(0), -q(0),
    //        -q(1), q(0), typename Derived::Scalar(0);
    //    return ans;
    //}
}