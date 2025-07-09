# @file:	CMakeLists.txt
# @author:	Jacob Xie
# @date:	2025/07/09 23:35:53 Wednesday
# @brief:

cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

project(SparklingGraffitoTest)

find_package(SparklingGraffito REQUIRED PATHS /opt/SparklingGraffito-0.0.0)
message(STATUS "Found SparklingGraffito: ${SparklingGraffito_DIR}")

add_executable(test_req test_req.cpp)
target_link_libraries(test_req PRIVATE SparklingGraffito::SparklingGraffito)

add_executable(test_rep test_rep.cpp)
target_link_libraries(test_rep PRIVATE SparklingGraffito::SparklingGraffito)

