cmake_minimum_required(VERSION 3.10)
project(Cam2GyroCalib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 3rd lib
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

# opencv
include_directories(${OpenCV_INCLUDE_DIRS})
# vendor
set(VENDOR_DIR vendor)
# glm
set(GLM_SOURCE_DIR ${VENDOR_DIR}/glm)
# imgui
set(IMGUI_SOURCE_DIR ${VENDOR_DIR}/imgui)
file(GLOB IMGUI_SOURCE_FILE  "${IMGUI_SOURCE_DIR}/*.cpp")
# stb
set(STB_SOURCE_DIR ${VENDOR_DIR}/stb_image)
file(GLOB STB_SOURCE_FILE  "${STB_SOURCE_DIR}/*.cpp")
# glad
set(GLAD_DIR ${VENDOR_DIR}/Glad)
set(GLAD_INCLUDE_DIR ${GLAD_DIR}/include)
add_subdirectory(${GLAD_DIR} GLAD_BUILD)

#glfw
set(GLFW_DIR ${VENDOR_DIR}/GLFW)
set(GLFW_INCLUDE_DIR ${GLFW_DIR}/include)
add_subdirectory(${GLFW_DIR} GLFW_BUILD)

# src
set(SRC_DIR src)
file(GLOB SRC_FILE  "${SRC_DIR}/*.cpp")
# src imgui
set(IMGUI_IMPL_DIR ${SRC_DIR}/ImGui)
file(GLOB IMGUI_IMPL_FILE  "${IMGUI_IMPL_DIR}/*.cpp")
# src tests
set(TESTS_DIR ${SRC_DIR}/tests)
file(GLOB TESTS_FILE  "${TESTS_DIR}/*.cpp")
# src tracking
set(SRC_TRACKING_DIR ${SRC_DIR}/tracking)
file(GLOB SRC_TRACKING_FILE "${SRC_TRACKING_DIR}/*.cpp")
# src net
set(SRC_NET_DIR ${SRC_DIR}/net)
file(GLOB SRC_NET_FILE "${SRC_NET_DIR}/*.cpp")

add_executable(Cam2GyroCalib
                ${SRC_FILE}
                ${IMGUI_IMPL_FILE}
                ${TESTS_FILE}
                ${SRC_TRACKING_FILE}
                ${SRC_NET_FILE}
                ${IMGUI_SOURCE_FILE}
                ${STB_SOURCE_FILE}
                )

target_include_directories(Cam2GyroCalib PRIVATE
                            ${SRC_DIR}
                            ${IMGUI_IMPL_DIR}
                            ${TESTS_DIR}
                            ${SRC_TRACKING_DIR}
                            ${SRC_NET_DIR}
                            ${GLFW_INCLUDE_DIR}
                            ${VENDOR_DIR}
                            ${GLM_SOURCE_DIR}
                            ${IMGUI_SOURCE_DIR}
                            ${STB_SOURCE_DIR}
                            ${GLAD_INCLUDE_DIR}
                            )

target_link_libraries(Cam2GyroCalib OpenGL::GL glfw glad ${OpenCV_LIBS} pcap m3api)
# target_compile_options(Cam2GyroCalib PRIVATE -g  -ggdb -O0 -Wall   -fstack-protector-all -march=native -fnon-call-exceptions)   # debug mode
target_compile_options(Cam2GyroCalib PRIVATE -O3 -Wall)   # release mode
                            