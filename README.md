# cpprobotparser
This module represents the robots.txt parser written in C++, which answers questions about whether or not a particular user agent can fetch a URL on the Web site that published the robots.txt file. For more details on the structure of robots.txt files, see http://www.robotstxt.org/orig.html.

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

### Example

```cpp
#include <iostream>
#include <cpprobotparser.hpp>

using namespace cpprobotparser;

int main(int, char**)
{
    RobotsTxtTokenizer tokenizer(
        R"(
        Sitemap: www.example.com/sitemap.xml

        User-agent: *
        Crawl-delay: 2.0

        User-agent: Googlebot

        Disallow: /oembed
        Disallow: /*/forks
        Disallow: /*/stars
        Disallow: /*/download
        Disallow: /*/revisions
        Disallow: /*/*/issues/new
        Disallow: /*/*/issues/search
        Disallow: /*/*/commits/*/*
        Disallow: /*/*/commits/*?author
        Disallow: /*/*/commits/*?path
        Disallow: /*/*/branches
        Disallow: /*/*/tags
        Disallow: /*/*/contributors
        Disallow: /*/*/comments

        Allow: /*/*/tree/master
        Allow: /*/*/blob/master

        User-agent: Yandex
        Allow: *
        Clean-param: ref /some_dir/get_book.pl
        )");

    const std::string sitemapUrl = tokenizer.sitemapUrl();

    std::cout << "Site map URL: " << (sitemapUrl.empty() ? "None" : sitemapUrl) << "\n\n";

    const std::vector<WellKnownUserAgent> wellKnownUserAgents =
        MetaRobotsHelpers::wellKnownUserAgents();

    for (WellKnownUserAgent wellKnownUserAgent : wellKnownUserAgents)
    {
        const std::vector<std::string> allowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenAllow);

        const std::vector<std::string> disallowTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenDisallow);

        const std::vector<std::string> cleanParamTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenCleanParam);

        const std::vector<std::string> crawlDelayTokens =
            tokenizer.tokenValues(wellKnownUserAgent, RobotsTxtToken::TokenCrawlDelay);

        if (allowTokens.empty() &&
            disallowTokens.empty() &&
            cleanParamTokens.empty() &&
            crawlDelayTokens.empty())
        {
            continue;
        }

        std::cout << "Tokens for " << MetaRobotsHelpers::userAgentString(wellKnownUserAgent) << ": \n";

        // print all allow tokens
        for (const std::string& allowTokenValue : allowTokens)
        {
            std::cout << "Allow: " << allowTokenValue << "\n";
        }

        // print all disallow tokens
        for (const std::string& disallowTokenValue : disallowTokens)
        {
            std::cout << "Disallow: " << disallowTokenValue << "\n";
        }

        // print all clean-param tokens
        for (const std::string& cleanParamTokenValue : cleanParamTokens)
        {
            std::cout << "Clean-param: " << cleanParamTokenValue << "\n";
        }

        // print all crawl-delay tokens
        for (const std::string& crawlDelayTokenValue : crawlDelayTokens)
        {
            std::cout << "Crawl-delay: " << crawlDelayTokenValue << "\n";
        }

        std::cout << "\n\n";
    }

    return 0;
}
```

#### Result
```
Site map URL: www.example.com/sitemap.xml

Tokens for googlebot:
Allow: /*/*/tree/master
Allow: /*/*/blob/master
Disallow: /oembed
Disallow: /*/forks
Disallow: /*/stars
Disallow: /*/download
Disallow: /*/revisions
Disallow: /*/*/issues/new
Disallow: /*/*/issues/search
Disallow: /*/*/commits/*/*
Disallow: /*/*/commits/*?author
Disallow: /*/*/commits/*?path
Disallow: /*/*/branches
Disallow: /*/*/tags
Disallow: /*/*/contributors
Disallow: /*/*/comments


Tokens for yandex:
Allow: *
Clean-param: ref /some_dir/get_book.pl


Tokens for *:
Crawl-delay: 2.0
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
    CMAKE_ARGS -DBUILD_TESTS=OFF -DMSVC_RUNTIME=${MSVC_RUNTIME}
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
