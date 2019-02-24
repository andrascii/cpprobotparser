cmake_minimum_required(VERSION 3.4)

set(CXX_URL_DEPENDENCY cxxurl CACHE INTERNAL "")
project(${CXX_URL_DEPENDENCY})

set(CMAKE_CXX_STANDARD 17)

#*********************************************************
# determine platform
#*********************************************************
set(PLATFORM UNKNOWN)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLATFORM LINUX)
    if (DEFINED ${ANDROID_PLATFORM})
        set(PLATFORM ANDROID)
        add_definitions(-DANDROID_PLATFORM)
    endif()
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM MAC_OS)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM WINDOWS)
endif()

message("Platform: " ${CMAKE_SYSTEM_NAME} "-" ${PLATFORM})

set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/url.hpp)

set(SOURCES
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/url.cpp)

if(MSVC)
    add_definitions(/MTd)
endif(MSVC)

add_library(${CXX_URL_DEPENDENCY} STATIC ${SOURCES})
set_target_properties(${CXX_URL_DEPENDENCY} PROPERTIES SOVERSION 1)
set_target_properties(${CXX_URL_DEPENDENCY} PROPERTIES PUBLIC_HEADER url.hpp)