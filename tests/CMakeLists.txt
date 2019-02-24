cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

set(TESTS tests)
project(${TESTS})

add_executable(
	${TESTS}
	tests.cpp
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/std:c++17
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
		"${CPPROBOTPARSER_OUTPUT_DIR}/${CPPROBOTPARSER_LIBRARY}.dll"
		"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
	)
endif()

include_directories(${GOOGLE_TEST_HEADERS_DIR} ${CPPROBOTPARSER_INCLUDE_DIR})

target_link_libraries(${TESTS}
	${CPPROBOTPARSER_LIBRARY}
	"$<$<CONFIG:Debug>:${GTEST_DEBUG_PATH}/gtestd.lib>"
	"$<$<CONFIG:Debug>:${GTEST_DEBUG_PATH}/gtest_maind.lib>"
	"$<$<CONFIG:Release>:${GTEST_RELEASE_PATH}/gtest.lib>"
	"$<$<CONFIG:Release>:${GTEST_RELEASE_PATH}/gtest_main.lib>"
)