#
# This file exports the next variables to link with it:
# CPPROBOTPARSER_LIBRARY - this library dependency and the name of the library. Use target_link_libraries with this variable
# CPPROBOTPARSER_INCLUDE_DIR - path to the include files
# CPPROBOTPARSER_BINARY_DIR - path to the binary files
#

cmake_minimum_required(VERSION 3.2)

set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)
set(CMAKE_CXX_STANDARD 17)

include(cmake/configure_msvc_runtime.cmake)
include(cmake/add_msvc_precompiled_header.cmake)
include(cmake/cotire.cmake)

#add_subdirectory(third_party)

option(BUILD_TESTS "Build 'tests' project" ON)
option(BUILD_AS_SHARED "Forces building cpprobotparser library as dynamic load library" OFF)
option(USE_DYNAMIC_CXX_RUNTIME "Forces building cpprobotparser with the dynamic C++ runtime library" OFF)

set(CPPROBOTPARSER_LIBRARY cpprobotparser)
project(${CPPROBOTPARSER_LIBRARY})

set(SOURCES_DIR src)
set(INCLUDE_DIR include)
set(CXXURL_INCLUDE_DIR "./third_party/cxxurl/")

aux_source_directory(${SOURCES_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "include/*.h")

list(APPEND SOURCES_LIST ${CXXURL_SOURCES_LIST})
list(APPEND HEADERS_LIST ${CXXURL_HEADERS_LIST})

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" SOURCES_LIST)
source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

message(STATUS "==== CXXURL_HEADERS_LIST: ${CXXURL_HEADERS_LIST}")
message(STATUS "==== CXXURL_SOURCES_LIST: ${CXXURL_SOURCES_LIST}")
source_group("cxxurl" FILES ${CXXURL_HEADERS_LIST} ${CXXURL_SOURCES_LIST})

if (BUILD_AS_SHARED)

    add_library(
        ${CPPROBOTPARSER_LIBRARY} SHARED
        ${SOURCES_LIST}
        ${HEADERS_LIST}
    )

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(CPPROBOTPARSER_BIN_EXTENSION dll)
    else()
        set(CPPROBOTPARSER_BIN_EXTENSION so)
    endif()

else()

    add_library(
        ${CPPROBOTPARSER_LIBRARY} STATIC
        ${SOURCES_LIST}
        ${HEADERS_LIST}
    )

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(CPPROBOTPARSER_BIN_EXTENSION lib)
    else()
        set(CPPROBOTPARSER_BIN_EXTENSION a)
    endif()

    add_definitions(-D${CPPROBOTPARSER_LIBRARY}_STATIC)

endif()

if (MSVC)
	add_definitions(
		/EHsc
		/MP
		/Zi
		/W4
		/WX
	)

	set_target_properties(
		${CPPROBOTPARSER_LIBRARY}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
endif()

configure_msvc_runtime()

include_directories(${INCLUDE_DIR} ${CXXURL_INCLUDE_DIR})
target_link_libraries(${CPPROBOTPARSER_LIBRARY})

# set additional export variables
set(CPPROBOTPARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "")
set(CPPROBOTPARSER_SINGLE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/single_include" CACHE INTERNAL "")
set(CPPROBOTPARSER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}" CACHE INTERNAL "")

# configuration data output
message("=======================${CPPROBOTPARSER_LIBRARY}================================")
message(STATUS "==== CPPROBOTPARSER_INCLUDE_DIR: ${CPPROBOTPARSER_INCLUDE_DIR}")
message(STATUS "==== CPPROBOTPARSER_BINARY_DIR: ${CPPROBOTPARSER_BINARY_DIR}")
message(STATUS "==== CPPROBOTPARSER_LIBRARY: ${CPPROBOTPARSER_LIBRARY}")
message(STATUS "==== CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "==== CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message("================================================================================")

if (BUILD_TESTS)
#    add_subdirectory(tests)
endif()

if(NOT WIN32)
	set_target_properties(${CPPROBOTPARSER_LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/stdafx.h")
	cotire(${CPPROBOTPARSER_LIBRARY})
endif()

set(CPPROBOTPARSER_INCLUDE_DESTINATION "include/${CPPROBOTPARSER_LIBRARY}/include/")
set(CPPROBOTPARSER_LIBRARY_DESTINATION "lib/${CPPROBOTPARSER_LIBRARY}/${CMAKE_BUILD_TYPE}")

install(TARGETS ${CPPROBOTPARSER_LIBRARY} EXPORT ${CPPROBOTPARSER_LIBRARY} DESTINATION ${CPPROBOTPARSER_LIBRARY_DESTINATION})
install(EXPORT ${CPPROBOTPARSER_LIBRARY} DESTINATION ${CPPROBOTPARSER_LIBRARY_DESTINATION})
install(FILES ${HEADERS_LIST} DESTINATION ${CPPROBOTPARSER_INCLUDE_DESTINATION})
