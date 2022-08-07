cmake_minimum_required(VERSION 3.4.1)

SET(C_LIB native-lib)

add_library(${C_LIB}
        SHARED
        android.cpp)

find_library(log-lib log)
target_link_libraries(${C_LIB} ${log-lib})