cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

add_subdirectory(googletest)
set(GTEST ${GTEST_DEPENDENCY} CACHE INTERNAL "")

set(TESTS tests)
project(${TESTS})

unset(SOURCES_LIST)
unset(HEADERS_LIST)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(
	${TESTS}
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
		/MTd
	)

	add_custom_command(
		TARGET ${TESTS} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
		"${CPPROBOTPARSER_BINARY_DIR}/${CPPROBOTPARSER_LIBRARY}.lib"
		"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
	)
endif()

include_directories(${GOOGLE_TEST_HEADERS_DIR} ${CPPROBOTPARSER_INCLUDE_DIR})
add_dependencies(${TESTS} ${CPPROBOTPARSER_LIBRARY} ${GTEST})

target_link_libraries(${TESTS}
	${CPPROBOTPARSER_LIBRARY}
	"$<$<CONFIG:Debug>:${GTEST_DEBUG_PATH}/gtestd.lib>"
	"$<$<CONFIG:Debug>:${GTEST_DEBUG_PATH}/gtest_maind.lib>"
	"$<$<CONFIG:Release>:${GTEST_RELEASE_PATH}/gtest.lib>"
	"$<$<CONFIG:Release>:${GTEST_RELEASE_PATH}/gtest_main.lib>"
)