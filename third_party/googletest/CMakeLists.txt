cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

include(ExternalProject)

externalproject_add(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG master
    SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(googletest Source_Dir Binary_Dir)
set(GOOGLETEST_HEADERS_DIR "${Source_Dir}/googletest/include" CACHE INTERNAL "")
include_directories(${GOOGLETEST_HEADERS_DIR})

message(STATUS "GOOGLETEST Binary directory ${Binary_Dir}")

set(GOOGLETEST_DEPENDENCY googletest CACHE INTERNAL "")

# set(CurlDebugDir ${Binary_Dir}/lib/debug)
# set(CurlReleaseDir ${Binary_Dir}/lib/release)

# message(STATUS "CURL Debug directory ${CurlDebugDir}")
# message(STATUS "CURL Release directory ${CurlReleaseDir}")