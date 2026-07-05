cmake_minimum_required(VERSION 3.15)
project(redmagicfancontrol C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.c
    src/config.c
    src/runtime/engine.c
    src/external/cjson/cJSON.c
    src/fan.c
    src/socket.c
    src/logging.c
    src/runtime/runtime.c
)

# Include directories
include_directories(
    src
    src/config
    src/engine
    src/external/cjson
    src/fan
    src/ipc
    src/logging
    src/runtime
    
)

# Create executable
add_executable(redmagicfancontrold ${SOURCES})

# Link libraries
# cJSON is included as source (`src/external/cjson/cJSON.c`) so
# no external `-lcjson` link is necessary. Keep this section
# available for future external libs.
# target_link_libraries(redmagicfancontrold PRIVATE cjson)

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
