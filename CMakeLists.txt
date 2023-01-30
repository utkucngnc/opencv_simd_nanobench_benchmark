cmake_minimum_required(VERSION 3.13)
project(ocv_simd_nanobench)

find_package(OpenCV REQUIRED)

#CREATE EXECUTABLE
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    Simd
    nanobench
    )