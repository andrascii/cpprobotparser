cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

if (USE_DYNAMIC_CXX_RUNTIME)
    set(gtest_force_shared_crt ON)
else()
    set(gtest_force_shared_crt OFF)
endif()

include(ExternalProject)

set(GTEST_DEPENDENCY googletest CACHE INTERNAL "")

externalproject_add(${GTEST_DEPENDENCY}
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG master
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DEPENDENCY}-src"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DEPENDENCY}-build"
    CMAKE_ARGS -Dgtest_force_shared_crt=${gtest_force_shared_crt} # force building gtest with the same C++ runtime library version (dynamic or static)
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(${GTEST_DEPENDENCY} Source_Dir Binary_Dir)
set(GTEST_HEADERS_DIR "${Source_Dir}/${GTEST_DEPENDENCY}/include" CACHE INTERNAL "")
include_directories(${GTEST_HEADERS_DIR})

# export variables
set(GTEST_DEBUG_PATH "${Binary_Dir}/lib/Debug" CACHE INTERNAL "")
set(GTEST_RELEASE_PATH "${Binary_Dir}/lib/Release" CACHE INTERNAL "")

# debug info
message("===========================${GTEST_DEPENDENCY}==============================")
message(STATUS "==== GTEST_DEBUG_PATH: ${GTEST_DEBUG_PATH}")
message(STATUS "==== GTEST_RELEASE_PATH: ${GTEST_RELEASE_PATH}")
message("============================================================================")