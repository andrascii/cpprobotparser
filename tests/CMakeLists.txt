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
		/std:c++latest
		/Zi
		/W4
		/WX
		/MTd
	)
	
	set_target_properties(
		${TESTS}
		PROPERTIES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP /SUBSYSTEM:CONSOLE"
	)
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
#add_subdirectory(#"${CMAKE_CURRENT_BINARY_DIR}/../third_party/googletest/googletest-src"
                 #"${CMAKE_CURRENT_BINARY_DIR}/../third_party/googletest/googletest-build"
                 #EXCLUDE_FROM_ALL)

include_directories(${GOOGLETEST_HEADERS_DIR})
target_link_libraries(${TESTS} gtest_main gmock_main gtest gmock)