unset(SOURCES_LIST)
unset(HEADERS_LIST)

file(GLOB_RECURSE SOURCES_LIST "cxxurl/*.cpp")
file(GLOB_RECURSE HEADERS_LIST "cxxurl/*.hpp")

set(CXXURL_SOURCES_LIST ${SOURCES_LIST} CACHE INTERNAL "")
set(CXXURL_HEADERS_LIST ${HEADERS_LIST} CACHE INTERNAL "")
set(CXXURL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cxxurl" CACHE INTERNAL "")