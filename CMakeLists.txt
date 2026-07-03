cmake_minimum_required(VERSION 3.15)
project(CrossPlatformApp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(my_binary src/main.c)

# CRITICAL: Force 16 KB page alignment exclusively for Android targets
if(ANDROID)
    target_link_options(my_binary PRIVATE "-Wl,-z,max-page-size=16384")
endif()
