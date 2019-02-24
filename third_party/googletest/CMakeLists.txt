cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

include(ExternalProject)

externalproject_add(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG master
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(googletest Source_Dir Binary_Dir)
set(GOOGLE_TEST_HEADERS_DIR "${Source_Dir}/googletest/include" CACHE INTERNAL "")
include_directories(${GOOGLE_TEST_HEADERS_DIR})

# export variables
set(GTEST_DEBUG_PATH "${Binary_Dir}/lib/Debug" CACHE INTERNAL "")
set(GTEST_RELEASE_PATH "${Binary_Dir}/lib/Release" CACHE INTERNAL "")

# debug info
message("==== Binary_Dir: ${Binary_Dir}")
message("==== GTEST_DEBUG_PATH: ${GTEST_DEBUG_PATH}")
message("==== GTEST_RELEASE_PATH: ${GTEST_RELEASE_PATH}")