cmake_minimum_required(VERSION 3.4.1)

SET(C_LIB native-lib)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

file(GLOB PPG
        "ppg/*.h"
        "ppg/*.cpp")

add_library(${C_LIB}
        SHARED
        ${PPG}
        android.cpp)

find_library(log-lib log)
target_link_libraries(${C_LIB} ${log-lib})