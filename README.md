# cpprobotparser
This module represents the robots.txt parser written in C++, which answers questions about whether or not a particular user agent can fetch a URL on the Web site that published the robots.txt file. For more details on the structure of robots.txt files, see http://www.robotstxt.org/orig.html.

## RobotsTxtTokenizer

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
#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"
#include "well_known_user_agent.h"
#include "robots_txt_token.h"

using namespace cpprobotparser;

int main(int, char**)
{
    RobotsTxtTokenizer tokenizer =
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
        )"_tokenizeRobotsTxt;

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

    std::cin.get();
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
