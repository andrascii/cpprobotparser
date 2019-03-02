#
# This file exports the next variables to link with it:
# CPPROBOTPARSER_LIBRARY - this library dependency and the name of the library. Use target_link_libraries with this variable
# CPPROBOTPARSER_INCLUDE_DIR - path to the include files
# CPPROBOTPARSER_BINARY_DIR - path to the binary files
#

cmake_minimum_required(VERSION 3.2)

set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

include(cmake/configure_msvc_runtime.cmake)
include(cmake/add_msvc_precompiled_header.cmake)

add_subdirectory(third_party)

option(BUILD_TESTS "Build tests project" ON)

set(CPPROBOTPARSER_LIBRARY cpprobotparser)
project(${CPPROBOTPARSER_LIBRARY})

set(SOURCES_DIR src)
set(INCLUDE_DIR include)
set(CXXURL_INCLUDE_DIR "./third_party/cxxurl/")

aux_source_directory(${SOURCES_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "include/*.h")

# work with precompiled headers in MSVC
list(REMOVE_ITEM SOURCES_LIST src/stdafx.cpp)
add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" SOURCES_LIST)

source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

add_library(
	${CPPROBOTPARSER_LIBRARY} STATIC
	${SOURCES_LIST}
	${HEADERS_LIST}
)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
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

set_target_properties(
	${CPPROBOTPARSER_LIBRARY}
	PROPERTIES
	PUBLIC_HEADER "robots_txt_rules.h"
	PUBLIC_HEADER "robots_txt_tokenizer.h"
	PUBLIC_HEADER "robots_txt_token.h"
	PUBLIC_HEADER "well_known_user_agent.h"
)

configure_msvc_runtime()

include_directories(${INCLUDE_DIR} ${CXXURL_INCLUDE_DIR})
target_link_libraries(${CPPROBOTPARSER_LIBRARY} ${THIRD_PARTY_CXX_URL})

# set additional export variables
set(CPPROBOTPARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "")
set(CPPROBOTPARSER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}" CACHE INTERNAL "")

# debug output
message("==== ${CPPROBOTPARSER_LIBRARY} debug info:")
message("==== CPPROBOTPARSER_INCLUDE_DIR: ${CPPROBOTPARSER_INCLUDE_DIR}")
message("==== CPPROBOTPARSER_BINARY_DIR: ${CPPROBOTPARSER_BINARY_DIR}")
message("==== CPPROBOTPARSER_LIBRARY: ${CPPROBOTPARSER_LIBRARY}")

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()