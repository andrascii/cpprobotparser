# cpprobotparser
This module represents the robots.txt parser written in C++, which answers questions about whether or not a particular user agent can fetch a URL on the Web site that published the robots.txt file. For more details on the structure of robots.txt files, see http://www.robotstxt.org/orig.html.

## How to determine which URLs are allowed?

The library is exported [`RobotsTxtRules`](https://github.com/andrascii/cpprobotparser/blob/master/include/robots_txt_rules.h).
Below is the example of using `RobotsTxtRules`.

### Example

```cpp
#include <iostream>
#include <cpprobotparser.hpp>

using namespace cpprobotparser;

int main(int, char**)
{
    const RobotsTxtRules rules(R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: Googlebot

        Disallow: /oembedB
        Disallow: /*/forks
        Disallow: /*/*/commits/*/*
        Disallow: /*/*/commits/*?author
        Disallow: /*/*/commits/*?path

        Allow: /*/*/tree/master
        Allow: /*/*/blob/master)");

    const std::vector<std::string> urlToCheck
    {
        "http://example.com/index.php",
        "http://example.com/oembedB",
        "http://example.com/folder1/folder2/commits/article.php?author",
        "http://example.com/1/2/blob/master"
    };

    std::cout << "Rules for GoogleBot:\n";

    for (const std::string& url : urlToCheck)
    {
        const bool isAllowed = rules.isUrlAllowed(url, WellKnownUserAgent::GoogleBot);
        std::cout << "The URL: " << url << (isAllowed ? " is allowed" : " is not allowed") << "\n";
    }

    return 0;
}
```

#### Result
```
Rules for GoogleBot:
The URL: http://example.com/index.php is allowed
The URL: http://example.com/oembedB is not allowed
The URL: http://example.com/folder1/folder2/commits/article.php?author is not allowed
The URL: http://example.com/1/2/blob/master is allowed
```

## How to tokenize the robots.txt file

[`robots_txt_tokenizer.h`](https://github.com/andrascii/cpprobotparser/blob/master/include/robots_txt_tokenizer.h) - this tokenizer is used to only parse robots.txt file and provide you rules for each user agent occurred in the file.

Using `tokenValues` method it can give the value of any token taken from this enumeration [`robots_txt_token.h`](https://github.com/andrascii/cpprobotparser/blob/master/include/robots_txt_token.h):

```cpp
namespace cpprobotparser
{

enum class RobotsTxtToken
{
    TokenUserAgent,
    TokenAllow,
    TokenDisallow,
    TokenSitemap,
    TokenHost,
    TokenCrawlDelay,
    TokenCleanParam,
    TokenCommentary,
    TokenStringDelimeter,
    TokenUnknown
};

}
```

## Example Of Incorporating Into An Existing CMake Project Using MSVC

Assume that we have a project which consists of one `src` folder:

```cmake
cmake_minimum_required(VERSION 3.2)

include(ExternalProject)

# we will link cpprobotparser with a dynamic runtime C++ version of library
# in order to avoid link errors
set(MSVC_RUNTIME dynamic)

externalproject_add(cpprobotparser
    GIT_REPOSITORY https://github.com/andrascii/cpprobotparser
    GIT_TAG master
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpprobotparser-src"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpprobotparser-build"
    CMAKE_ARGS -DBUILD_TESTS=OFF -DUSE_DYNAMIC_CXX_RUNTIME=ON -DBUILD_AS_SHARED=ON
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

externalproject_get_property(cpprobotparser Source_Dir Binary_Dir)

# Path to the single `cpprobotparser.hpp` header file
set(CPPROBOTPARSER_INCLUDE_DIR "${Source_Dir}/single_include")

# Path to the debug binary directory created by default of microsoft compiler
set(CPPROBOTPARSER_DEBUG_BINARY_DIR "${Binary_Dir}/Debug")

# Path to the release binary directory created by default of microsoft compiler
set(CPPROBOTPARSER_RELEASE_BINARY_DIR "${Binary_Dir}/Release")

set(TEST_PROJECT testproject)
project(${TEST_PROJECT})

set(SOURCES_DIR src)

# collect sources and headers of our main project
aux_source_directory(${SOURCES_DIR} SOURCES_LIST)
file(GLOB_RECURSE HEADERS_LIST "src/*.h")

add_executable(
	${TEST_PROJECT}
	${SOURCES_LIST}
	${HEADERS_LIST}
)

set(CMAKE_CXX_STANDARD 17)

include_directories(${INCLUDE_DIR} ${CPPROBOTPARSER_INCLUDE_DIR})

# firstly we will build the cpprobotparser
add_dependencies(${TEST_PROJECT} cpprobotparser)

# finally link our project with a cpprobotparser.lib
target_link_libraries(${TEST_PROJECT}
    "$<$<CONFIG:Debug>:${CPPROBOTPARSER_DEBUG_BINARY_DIR}/cpprobotparser.lib>" # for debug version
    "$<$<CONFIG:Release>:${CPPROBOTPARSER_RELEASE_BINARY_DIR}/cpprobotparser.lib>" # for release version
)

```
