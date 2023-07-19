cmake_minimum_required(VERSION 3.10)
project(udpmonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

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


add_executable(udpmonitor
                ${SRC_FILE}
                ${IMGUI_IMPL_FILE}
                ${TESTS_FILE}
                ${IMGUI_SOURCE_FILE}
                ${STB_SOURCE_FILE}
                )

target_include_directories(udpmonitor PRIVATE
                            ${SRC_DIR}
                            ${IMGUI_IMPL_DIR}
                            ${TESTS_DIR}
                            ${GLFW_INCLUDE_DIR}
                            ${VENDOR_DIR}
                            ${GLM_SOURCE_DIR}
                            ${IMGUI_SOURCE_DIR}
                            ${STB_SOURCE_DIR}
                            ${GLAD_INCLUDE_DIR}
                            )

target_link_libraries(udpmonitor OpenGL::GL glfw glad)
# target_compile_options(udpmonitor PRIVATE -g  -ggdb -O0 -Wall   -fstack-protector-all -march=native -fnon-call-exceptions)   # debug mode
target_compile_options(udpmonitor PRIVATE -O3 -Wall)   # release mode
                            