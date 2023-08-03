cmake_minimum_required(VERSION 3.10)
project(glad VERSION 3.4.0 LANGUAGES C)

add_library(glad STATIC
            "include/glad/glad.h"
            "include/KHR/khrplatform.h"
            "src/glad.c"
)

target_include_directories(glad PRIVATE
                            "include"
)