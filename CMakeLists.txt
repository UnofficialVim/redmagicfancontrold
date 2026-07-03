cmake_minimum_required(VERSION 3.15)
project(redmagicfancontrol C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.c
    src/config/config.c
    src/fan/fan.c
    src/ipc/socket.c
    src/logging/logging.c
)

# Include directories
include_directories(
    src
    src/config
    src/fan
    src/ipc
    src/logging
    external/cjson
)

# Add external libraries
add_library(cjson STATIC external/cjson/cJSON.c)

# Create executable
add_executable(redmagicfancontrold ${SOURCES})

# Link libraries
target_link_libraries(redmagicfancontrold PRIVATE cjson)

# Platform-specific settings
if(ANDROID)
    # Android-specific configuration
    message(STATUS "Building for Android")
    target_link_options(redmagicfancontrold PRIVATE "-Wl,-z,max-page-size=16384")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # Linux-specific configuration
    message(STATUS "Building for Linux")
    if(NOT ANDROID)
        # Additional Linux-specific flags if needed
        target_compile_options(redmagicfancontrold PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
