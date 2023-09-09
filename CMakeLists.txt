cmake_minimum_required(VERSION 3.14)
project(shale)

set(CMAKE_CXX_STANDARD 14)

add_executable(shale main.cpp AdSupply.cpp AdSupply.h AdDemand.cpp AdDemand.h Shale.cpp Shale.h)