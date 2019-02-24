add_subdirectory(cxxurl)
add_subdirectory(googletest)

set(THIRD_PARTY_CXX_URL ${CXX_URL_DEPENDENCY} CACHE INTERNAL "")
set(THIRD_PARTY_GTEST ${GTEST_DEPENDENCY} CACHE INTERNAL "")